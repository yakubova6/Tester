import React, { useEffect, useState } from 'react';
import axios from 'axios';
import ErrorPage from './errors/ErrorPage';
import { useNavigate } from 'react-router-dom';
import './styles/Dashboard.css'; 

const Dashboard = () => {
    const [userData, setUserData] = useState(null);
    const [error, setError] = useState(false);
    const [loading, setLoading] = useState(true);
    const navigate = useNavigate();

    useEffect(() => {
        const checkSession = async () => {
            try {
                const sessionResponse = await axios.get('http://localhost:5000/api/session', { withCredentials: true });
                if (sessionResponse.data.status === 'authorized') {
                    const userResponse = await axios.get('http://localhost:5000/api/user-data', { withCredentials: true });
                    setUserData(userResponse.data);
                } else {
                    navigate('/'); // Перенаправляем на домашнюю страницу
                }
            } catch (err) {
                console.error('Ошибка проверки сессии:', err);
                setError(true); // Устанавливаем флаг ошибки
            } finally {
                setLoading(false); // Устанавливаем состояние загрузки в false
            }
        };

        checkSession();
    }, [navigate]);

    if (loading) return <div>Загрузка...</div>;
    if (error) return <ErrorPage />;

    return (
        <div className="dashboard-container">
            <h2>Личный кабинет</h2>
            {userData ? (
                <>
                    <p className="dashboard-text">Имя: {userData.name}</p>
                    <button className="logout-button" onClick={() => navigate('/logout')}>Выйти</button>
                </>
            ) : (
                <p className="dashboard-text">Данные пользователя не загружены.</p>
            )}
        </div>
    );
};

export default Dashboard;