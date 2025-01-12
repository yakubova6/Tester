import React, { useState } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';
import '../styles/Login.css';

const CodeAuth = ({ setUserStatus }) => {
    const [authCode, setAuthCode] = useState(''); // Код авторизации от сервера
    const [code, setCode] = useState(''); // Поле ввода кода
    const [error, setError] = useState('');
    const navigate = useNavigate();

    // Функция для получения кода авторизации
    const handleRequestCode = async () => {
        try {
            console.log('Отправляем запрос на генерацию кода...');
            const responseCode = await axios.post('/api/auth/code'); // Используем обновленный API
            console.log('Ответ от сервера при получении кода:', responseCode.data);
            
            if (responseCode.data && responseCode.data.code) {
                setAuthCode(responseCode.data.code); // Сохраняем код авторизации
                console.log('Сохранен код авторизации:', responseCode.data.code);
            } else {
                throw new Error('Ответ не содержит правильные данные');
            }
        } catch (err) {
            console.error("Ошибка при получении кода:", err);
            setError('Не удалось получить код. Попробуйте еще раз.');
        }
    };

    // Функция для авторизации после ввода кода
    const handleCodeSubmit = async (e) => {
        e.preventDefault();
        console.log('Функция handleCodeSubmit вызвана с кодом:', code);
        
        try {
            // Получаем sessionToken из куки
            const sessionToken = document.cookie.split('; ').find(row => row.startsWith('session_token='));
            const sessionTokenValue = sessionToken ? sessionToken.split('=')[1] : null;

            if (!sessionTokenValue) {
                setError('Сессионный токен отсутствует. Пожалуйста, получите код авторизации сначала.');
                return;
            }

            // Отправляем код и sessionToken на сервер
            console.log('Отправляем код на сервер:', { code, sessionTokenValue });
            
            const authResponse = await axios.post('/api/auth/verify-code', { code, sessionToken: sessionTokenValue }, { withCredentials: true });
            console.log('Ответ от сервера:', authResponse.data);
            
            if (authResponse.data.success) {
                const { userInfo } = authResponse.data; 
                console.log('Получены данные пользователя:', userInfo);

                setUserStatus('authorized'); // Обновляем статус пользователя
                navigate('/'); // Перенаправляем на главную страницу
            } else {
                setError('Авторизация не удалась. Попробуйте снова.');
                console.error('Авторизация не удалась:', authResponse.data);
            }
        } catch (err) {
            console.error("Ошибка при отправке кода:", err);
            setError('Неверный код или произошла ошибка. Попробуйте снова.');
        }
    };

    return (
        <div className="code-auth-container">
            <button onClick={handleRequestCode} className="social-button">
                Получить код авторизации
            </button>
            {authCode && <p>Ваш код авторизации: {authCode}</p>} {/* Отображаем код авторизации */}
            <form onSubmit={handleCodeSubmit}>
                <input
                    type="text"
                    className="input-code"
                    placeholder="Введите код"
                    value={code}
                    onChange={(e) => setCode(e.target.value)} // Обновляем состояние кода
                />
                <button type="submit" className="social-button">Авторизоваться</button>
            </form>
            {error && <p className="error-message">{error}</p>}
        </div>
    );
};

export default CodeAuth;