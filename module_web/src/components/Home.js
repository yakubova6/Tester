import React from 'react';
import './styles/Home.css'; 
import { useNavigate } from 'react-router-dom';
import welcomeGif from '../assets/images/welcome.gif'; // Импортируйте ваш GIF с правильным путем

const Home = () => {
    const navigate = useNavigate();

    const handleLogin = () => {
        navigate('/login'); // Перенаправление на страницу логина
    };

    return (
        <div className="container">
            <h1>
                Добро пожаловать!
                {' '}
                <img src={welcomeGif} alt="Welcome GIF" className="welcome-gif" />
            </h1>
            <p className="description">Приложение предназначено для проведения массовых опросов и тестирования, доступное как через веб-клиент, так и через Telegram-бота.</p>
            <p className="description">Для того, чтобы войти в него, нажмите кнопку ниже и используйте ваши учетные данные из внешних сервисов для аутентификации.</p>
            <p className="description">Мы рады видеть вас в нашем приложении!</p>
            <button className="login-button" onClick={handleLogin}>Войти</button>
        </div>
    );
};

export default Home;