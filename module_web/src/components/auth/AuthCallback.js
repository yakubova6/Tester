import React, { useEffect } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';

const AuthCallback = ({ setUserStatus }) => {
    const navigate = useNavigate();

    useEffect(() => {
        const params = new URLSearchParams(window.location.search);
        const code = params.get('code');
        const returnedState = params.get('state');
        const savedState = localStorage.getItem('auth_state');

        // Проверка наличия кода и соответствия состояния
        if (!code || returnedState !== savedState) {
            navigate('/login');
            return;
        }

        // Удаление сохраненного состояния
        localStorage.removeItem('auth_state');

        // Отправка POST-запроса на сервер
        axios.post('http://localhost:5000/api/auth/callback', { code, state: returnedState })
            .then((response) => {
                // Установка куки с токеном сессии
                document.cookie = `session_token=${response.data.sessionToken}; path=/; httpOnly; secure=${process.env.NODE_ENV === 'production'}`;

                // Сохранение токенов доступа и обновления в localStorage
                localStorage.setItem('accessToken', response.data.accessToken); // Сохраните accessToken
                localStorage.setItem('refreshToken', response.data.refreshToken); // Сохраните refreshToken
                
                // Обновление состояния пользователя
                setUserStatus('authorized'); // Установка статуса пользователя

                // Перенаправление на страницу Dashboard
                navigate('/dashboard');
            })
            .catch((error) => {
                console.error('Ошибка при авторизации:', error);
                navigate('/error'); // Перенаправление на страницу ошибки
            });
    }, [navigate, setUserStatus]);

    return <div>Обработка...</div>;
};

export default AuthCallback;