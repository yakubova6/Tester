import React, { useState } from 'react';
import axios from 'axios';

const CodeAuth = () => {
    const [code, setCode] = useState('');
    const [errorMessage, setErrorMessage] = useState('');

    const handleCodeAuth = () => {
        const sessionTokenRow = document.cookie.split('; ').find(row => row.startsWith('session_token='));
        const sessionToken = sessionTokenRow ? sessionTokenRow.split('=')[1] : null;

        axios.post('http://localhost:5000/api/code-auth', { code, sessionToken })
            .then(response => {
                console.log('Авторизация прошла успешно:', response.data);
                // Дополнительная логика после успешной авторизации
            })
            .catch(error => {
                console.error('Ошибка авторизации:', error);
                setErrorMessage('Ошибка авторизации. Пожалуйста, проверьте код.');
            });
    };

    return (
        <div>
            <input 
                type="text" 
                className="input-code"  
                placeholder="Введите код" 
                value={code} 
                onChange={(e) => setCode(e.target.value)} 
            />
            <button onClick={handleCodeAuth} className="social-button">Авторизоваться</button>
            {errorMessage && <p className="error-message">{errorMessage}</p>} {/* Отображаем сообщение об ошибке */}
        </div>
    );
};

export default CodeAuth;