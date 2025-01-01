// Вспомогательная функция для получения токена из localStorage
function getAccessToken() {
    return localStorage.getItem('accessToken');
}

// Функция для редиректа на страницу
function redirectTo(url) {
    window.location.href = url;
}

// Обработчик входа в систему
document.getElementById('login').addEventListener('click', () => {
    redirectTo('login/login.html');
});

// Обработчик авторизации
document.getElementById('login-github').addEventListener('click', () => {
    authenticate('github');
});

document.getElementById('login-yandex').addEventListener('click', () => {
    authenticate('yandex');
});

document.getElementById('login-code').addEventListener('click', () => {
    authenticate('code');
});

// Функция для аутентификации
function authenticate(type) {
    fetch(`/login?type=${type}`)
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                localStorage.setItem('accessToken', data.accessToken);
                redirectTo('dashboard.html');
            } else {
                alert('Ошибка авторизации');
            }
        });
}

// Загрузка информации о пользователе
if (document.getElementById('username')) {
    fetch('/api/user-info', {
        headers: {
            'Authorization': 'Bearer ' + getAccessToken()
        }
    })
    .then(response => response.json())
    .then(data => {
        document.getElementById('username').textContent = data.username;
        document.getElementById('user-disciplines').textContent = data.disciplines.join(', ');
    });
}

// Обработчик выхода из системы
document.getElementById('logout').addEventListener('click', () => {
    fetch('/logout', { method: 'POST' })
        .then(() => {
            localStorage.removeItem('accessToken');
            redirectTo('index.html');
        });
});