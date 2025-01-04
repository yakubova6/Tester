// GitHubAuth.js
import React from 'react';

const GitHubAuth = () => {
    const clientId = "Ov23libRIJj8xTzQLJsw"; 
    const redirectUri = "http://localhost:5000/auth/github/callback"; 
    const state = Math.random().toString(36).substring(2);

    const handleGitHubAuth = () => {
        // Перенаправляем пользователя на страницу авторизации GitHub
        window.location.href = `https://github.com/login/oauth/authorize?client_id=${clientId}&redirect_uri=${redirectUri}&state=${state}`;
    };

    return (
        <button onClick={handleGitHubAuth} className="social-button github">
            <i className="fab fa-github" style={{ marginRight: '8px' }}></i> {/* Иконка GitHub */}
            Войти через GitHub
        </button>
    );
};

export default GitHubAuth;