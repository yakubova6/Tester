// authService.js
const http = require('http');

// Функция для отправки запроса на авторизацию
async function sendAuthRequest(loginToken) {
    return new Promise((resolve, reject) => {
        const options = {
            hostname: 'localhost', // Хост заглушки
            port: 5000,           // Порт заглушки
            path: '/login',       // Путь для авторизации
            method: 'POST',       // Метод запроса
            headers: {
                'Content-Type': 'application/json'
            }
        };

        const req = http.request(options, (res) => {
            let data = '';
            res.on('data', (chunk) => {
                data += chunk;
            });
            res.on('end', () => {
                resolve(JSON.parse(data)); // Возвращаем ответ от сервера
            });
        });

        // Отправляем токен входа в теле запроса
        req.write(JSON.stringify({ loginToken }));
        req.end();
    });
}

// Функция для отправки запроса на логаут
async function sendLogoutRequest(refreshToken) {
    return new Promise((resolve, reject) => {
        const options = {
            hostname: 'localhost', // Хост заглушки
            port: 5000,           // Порт заглушки
            path: '/logout',      // Путь для логаута
            method: 'POST',       // Метод запроса
            headers: {
                'Content-Type': 'application/json'
            }
        };

        const req = http.request(options, (res) => {
            let data = '';
            res.on('data', (chunk) => {
                data += chunk;
            });
            res.on('end', () => {
                resolve(JSON.parse(data)); // Возвращаем ответ от сервера
            });
        });

        // Отправляем токен обновления в теле запроса
        req.write(JSON.stringify({ refreshToken }));
        req.end();
    });
}

// Экспортируем функции для использования в других файлах
module.exports = { sendAuthRequest, sendLogoutRequest };