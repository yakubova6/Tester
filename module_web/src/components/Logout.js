import React, { useEffect } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';

const Logout = ({ setUserStatus }) => {
    const navigate = useNavigate();

    const handleLogout = async () => {
        try {
            // Выполнение запроса на выход
            await axios.post('/api/logout', {}, { withCredentials: true });
            setUserStatus('unknown'); // Обновляем статус пользователя на 'unknown'
            navigate('/'); // Перенаправление на главную страницу после выхода
        } catch (error) {
            console.error('Ошибка выхода:', error);
            // Можно добавить логику для обработки ошибок, например, отображение сообщения пользователю
        }
    };

    // Вызов функции handleLogout сразу при загрузке компонента
    useEffect(() => {
        handleLogout();
    }, []);

    return <div>Выход из системы...</div>; // Можно добавить текст для отображения пользователю
};

export default Logout;