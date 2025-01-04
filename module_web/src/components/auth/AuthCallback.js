// AuthCallback.js
import React, { useEffect } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';

const AuthCallback = () => {
    const navigate = useNavigate();

    useEffect(() => {
        const params = new URLSearchParams(window.location.search);
        const code = params.get('code');
        const returnedState = params.get('state');
        const savedState = localStorage.getItem('github_auth_state');

        console.log('Полученные параметры:', { code, returnedState, savedState });

        // Проверяем, что code существует и state совпадает
        if (!code || returnedState !== savedState) {
            console.error('Отсутствуют необходимые параметры или состояние не совпадает:', { code, returnedState, savedState });
            navigate('/login'); // Перенаправляем на страницу входа
            return;
        }

        // Удаляем состояние из локального хранилища
        localStorage.removeItem('github_auth_state');

        // Отправляем код на сервер для получения токена
        axios.post('http://localhost:5000/api/auth/callback', { code, state: returnedState })
            .then((response) => {
                console.log('Ответ от сервера:', response.data);
                document.cookie = `session_token=${response.data.sessionToken}; path=/; secure; httponly`;
                navigate('/dashboard');
            })
            .catch((error) => {
                console.error('Ошибка при запросе к /api/auth/callback:', error.response ? error.response.data : error.message);
                navigate('/error');
            });
    }, [navigate]);

    return <div>Обработка...</div>;
};

export default AuthCallback;