import React, { useState } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';
import '../styles/Login.css';

const CodeAuth = ({ setUserStatus }) => {
    const [code, setCode] = useState('');
    const [error, setError] = useState('');
    const [receivedCode, setReceivedCode] = useState('');
    const [refreshToken, setRefreshToken] = useState(''); // Добавлен refreshToken
    const navigate = useNavigate();

    const handleRequestCode = async () => {
        try {
            const response = await axios.post('/api/auth/code');
            if (response.data && response.data.code) {
                setReceivedCode(response.data.code);
                setRefreshToken(response.data.refreshToken); // Предполагается, что refreshToken возвращается
                setError('');
            } else {
                throw new Error('Ответ не содержит код');
            }
        } catch (err) {
            console.error("Ошибка при получении кода:", err);
            setError('Не удалось получить код. Попробуйте еще раз.');
            setReceivedCode('');
        }
    };

    const handleCodeSubmit = async (e) => {
        e.preventDefault();
        try {
            const response = await axios.post('/api/auth/callback', { code, refreshToken });
            if (response.data) {
                setUserStatus('authorized');
                navigate('/dashboard');
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
            {receivedCode && <p>Ваш код: {receivedCode}</p>}
            <form onSubmit={handleCodeSubmit}>
                <input
                    type="text"
                    className="input-code"
                    placeholder="Введите код"
                    value={code}
                    onChange={(e) => setCode(e.target.value)}
                />
                <button type="submit" className="social-button">Авторизоваться</button>
            </form>
            {error && <p className="error-message">{error}</p>}
        </div>
    );
};

export default CodeAuth;