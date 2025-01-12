import React, { useEffect } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';
import Cookies from 'js-cookie';

const AuthCallback = ({ setUserStatus }) => {
    const navigate = useNavigate();

    useEffect(() => {
        const params = new URLSearchParams(window.location.search);
        const code = params.get('code');
        const returnedState = params.get('state');
        const savedState = localStorage.getItem('auth_state');

        // Проверка наличия кода и соответствия состояния
        if (!code || returnedState !== savedState) {
            console.error('Код авторизации отсутствует или состояние не совпадает.');
            navigate('/login');
            return;
        }

        // Удаление сохраненного состояния
        localStorage.removeItem('auth_state');
        
        console.log('Отправка кода:', code, 'и состояния:', returnedState);
        
        // Отправка POST-запроса на сервер
        axios.post('/api/auth/callback', { code, state: returnedState })
            .then((response) => {
                if (response.data && response.data.success) {
                    // Извлечение accessToken и refreshToken из ответа
                    const { accessToken, refreshToken } = response.data;
                    
                    // Сохранение токенов в куки
                    if (accessToken) {
                        Cookies.set('access_token', accessToken); // Сохраняем accessToken в куки
                        console.log('Сохранен токен доступа в куки:', accessToken);
                    }
                    if (refreshToken) {
                        Cookies.set('refresh_token', refreshToken); // Сохраняем refreshToken в куки
                        console.log('Сохранен токен обновления в куки:', refreshToken);
                    }

                    // Установка состояния пользователя
                    setUserStatus('authorized'); 
                    console.log('Авторизация успешна:', response.data);
                    
                    // Перенаправление на страницу Dashboard
                    navigate('/dashboard');
                } else {
                    console.error('Ошибка авторизации:', response.data);
                    navigate('/login'); // Перенаправление на страницу логина
                }
            })
            .catch((error) => {
                console.error('Ошибка при авторизации:', error);
                navigate('/error'); // Перенаправление на страницу ошибки
            });
    }, [navigate, setUserStatus]);

    return null;
};

export default AuthCallback;