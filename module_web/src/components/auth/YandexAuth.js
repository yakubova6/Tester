import React from 'react';

const YandexAuth = () => {
    const clientId = process.env.REACT_APP_YANDEX_CLIENT_ID; 
    const redirectUri = process.env.REACT_APP_YANDEX_REDIRECT_URI; 
    const state = `yandex_${Math.random().toString(36).substring(2)}`;

    const handleYandexAuth = () => {
        localStorage.setItem('auth_state', state);
        window.location.href = `https://oauth.yandex.ru/authorize?response_type=code&client_id=${clientId}&redirect_uri=${redirectUri}&state=${state}`;
    };

    return (
        <button onClick={handleYandexAuth} className="social-button yandex">
            <i className="fab fa-yandex" style={{ marginRight: '8px' }}></i>
            Войти через Яндекс ID
        </button>
    );
};

export default YandexAuth;