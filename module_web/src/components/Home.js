import React from 'react';
import './styles/Home.css'; 
import { useNavigate } from 'react-router-dom';

const Home = () => {
    const navigate = useNavigate();

    const handleLogin = () => {
        navigate('/login'); // Перенаправление на страницу логина
    };

    return (
        <div className="container">
            <h1>Добро пожаловать в наше приложение!</h1>
            <p className="description">Здесь будет краткое описание нашего приложения.</p>
            <button className="login-button" onClick={handleLogin}>Войти</button>
        </div>
    );
};

export default Home;