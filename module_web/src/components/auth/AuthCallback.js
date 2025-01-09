import React, { useEffect } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';

// Ваш коллбэк на клиенте
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
        
        console.log('Отправка кода:', code, 'и состояния:', returnedState);
        // Отправка POST-запроса на сервер
        axios.post('/api/auth/callback', { code, state: returnedState })
            .then((response) => {
                // Установка состояния пользователя
                setUserStatus(response.data.status); // Устанавливаем статус пользователя на 'authorized'
                
                // Перенаправление на страницу Dashboard
                navigate('/dashboard');
            })
            .catch((error) => {
                console.error('Ошибка при авторизации:', error);
                navigate('/error'); // Перенаправление на страницу ошибки
            });
    }, [navigate, setUserStatus]);

    return null; // Или вы можете вернуть какой-то индикатор загрузки
};

export default AuthCallback;
