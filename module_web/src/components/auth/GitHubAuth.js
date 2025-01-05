import React from 'react';

const GitHubAuth = () => {
    const clientId = process.env.REACT_APP_GITHUB_CLIENT_ID; 
    const redirectUri = process.env.REACT_APP_GITHUB_REDIRECT_URI; 
    const state = `github_${Math.random().toString(36).substring(2)}`;

    const handleGitHubAuth = () => {
        localStorage.setItem('auth_state', state);
        window.location.href = `https://github.com/login/oauth/authorize?client_id=${clientId}&redirect_uri=${redirectUri}&state=${state}`;
    };

    return (
        <button onClick={handleGitHubAuth} className="social-button github">
            <i className="fab fa-github" style={{ marginRight: '8px' }}></i>
            Войти через GitHub
        </button>
    );
};

export default GitHubAuth;