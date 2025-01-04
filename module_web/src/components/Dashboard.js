import React, { useEffect, useState } from 'react';
import axios from 'axios';
import ErrorPage from './errors/ErrorPage';
import { useNavigate } from 'react-router-dom';

const Dashboard = () => {
    const [userData, setUserData] = useState(null);
    const [error, setError] = useState(false);
    const [loading, setLoading] = useState(true);
    const navigate = useNavigate();

    useEffect(() => {
        const getSessionToken = () => {
            const sessionTokenRow = document.cookie.split('; ').find(row => row.startsWith('session_token='));
            return sessionTokenRow ? sessionTokenRow.split('=')[1] : null;
        };
        console.log('Куки:', document.cookie);
        const sessionToken = getSessionToken();

        if (!sessionToken) {
            console.error('Токен не найден');
            navigate('/unauthorized');
            return;
        }

        console.log('Найден токен:', sessionToken); // Логируем токен
        setLoading(true);
        axios.get('/api/user-data', { headers: { Authorization: `Bearer ${sessionToken}` } })
            .then(response => {
                console.log('Ответ от /api/user-data:', response.data); // Логируем ответ
                if (response.status === 200) {
                    setUserData(response.data);
                } else {
                    throw new Error('Не удалось получить данные пользователя');
                }
            })
            .catch(err => {
                console.error('Ошибка при получении данных пользователя:', err);
                if (err.response) {
                    console.log('Статус ответа:', err.response.status); // Логируем статус ответа
                    if (err.response.status === 401) {
                        navigate('/unauthorized');
                    } else if (err.response.status === 403) {
                        navigate('/forbidden');
                    } else {
                        setError(true);
                    }
                } else {
                    setError(true);
                }
            })
            .finally(() => {
                setLoading(false);
            });
    }, [navigate]);

    if (loading) return <div>Загрузка...</div>;
    if (error) return <ErrorPage />;

    const handleLogout = () => {
        navigate('/logout'); // Переход на страницу выхода
    };

    return (
        <div>
            <h2>Личный кабинет</h2>
            <p>Имя: {userData.name}</p>
            <button onClick={handleLogout}>Выйти</button>
        </div>
    );
};

export default Dashboard;