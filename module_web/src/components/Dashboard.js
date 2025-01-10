import React, { useEffect, useState } from 'react';
import axios from 'axios';
import ErrorPage from './errors/ErrorPage';
import { useNavigate } from 'react-router-dom';
import './styles/Dashboard.css';

const Dashboard = () => {
    const [userData, setUserData] = useState({
        first_name: '',
        last_name: '',
        middle_name: '',
        role: ''
    });
    const [error, setError] = useState(false);
    const [loading, setLoading] = useState(true);
    const [isEditing, setIsEditing] = useState(false);
    const [formData, setFormData] = useState({
        first_name: '',
        last_name: '',
        middle_name: ''
    });
    const [disciplines, setDisciplines] = useState([]);
    const navigate = useNavigate();

    useEffect(() => {
        const checkSession = async () => {
            try {
                const sessionResponse = await axios.get('/api/session', { withCredentials: true });
                console.log('Проверка сессии:', sessionResponse.data);

                if (sessionResponse.data.status === 'authorized') {
                    const token = sessionResponse.data.token; // Предполагается, что токен возвращается здесь
                    const userId = sessionResponse.data.userId; // Получаем userId из ответа сессии
                    await fetchUserData(userId, token); // Передаем userId для получения данных
                    await fetchDisciplines(token); // Получаем дисциплины
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

        const fetchUserData = async (userId, token) => {
            try {
                const userResponse = await axios.get(`/api/users`, { headers: { Authorization: `Bearer ${token}` } });
                console.log('Ответ от API пользователя:', userResponse.data); // Логируем ответ от API

                // Получаем первого пользователя из массива users
                const user = userResponse.data.users[0]; // Извлекаем первого пользователя

                if (user) {
                    // Устанавливаем данные пользователя
                    setUserData({
                        id: user.id, // Добавляем id пользователя
                        first_name: user.first_name,
                        last_name: user.last_name,
                        middle_name: user.middle_name,
                        role: 'Неизвестно' // Устанавливаем роль как "Неизвестно"
                    });
                    // Устанавливаем данные для редактирования
                    setFormData({
                        first_name: user.first_name,
                        last_name: user.last_name,
                        middle_name: user.middle_name
                    });

                    // Логируем состояния
                    console.log('Состояние userData после установки:', {
                        first_name: user.first_name,
                        last_name: user.last_name,
                        middle_name: user.middle_name
                    });
                    console.log('Состояние formData после установки:', {
                        first_name: user.first_name,
                        last_name: user.last_name,
                        middle_name: user.middle_name
                    });
                } else {
                    console.error('Пользователь не найден');
                    setError(true);
                }
            } catch (error) {
                console.error('Ошибка при получении данных пользователя:', error.response ? error.response.data : error.message);
                setError(true);
            }
        };

        // Определение функции fetchDisciplines
        const fetchDisciplines = async (token) => {
            try {
                const disciplinesResponse = await axios.get('/api/disciplines', { headers: { Authorization: `Bearer ${token}` } });
                setDisciplines(disciplinesResponse.data); // Предполагается, что ответ содержит массив дисциплин
            } catch (error) {
                console.error('Ошибка при получении дисциплин:', error.response ? error.response.data : error.message);
                setError(true);
            }
        };

        checkSession();
    }, [navigate]);

    const handleChange = (e) => {
        const { name, value } = e.target;
        setFormData({ ...formData, [name]: value });
        console.log(`Изменение поля ${name}:`, value); // Логируем изменения полей формы
    };

    const handleEdit = () => {
        setIsEditing(true);
    };

    const handleSave = async () => {
        console.log('Текущие cookies:', document.cookie); // Логируем все cookies

        const sessionTokenRow = document.cookie.split('; ').find(row => row.startsWith('session_token'));
        const token = sessionTokenRow ? sessionTokenRow.split('=')[1] : null;

        console.log('Найденный токен:', token); // Логируем найденный токен

        try {
            const userId = userData.id; // Убедитесь, что userId существует
            if (!userId) {
                throw new Error('ID пользователя не найден'); // Добавьте проверку на случай, если userId не установлен
            }

            const fullName = {
                first_name: formData.first_name,
                last_name: formData.last_name,
                middle_name: formData.middle_name
            };

            console.log('Данные для отправки:', fullName); // Логируем данные перед отправкой

            // Отправляем измененные данные на новый эндпоинт
            const response = await axios.put(`/api/users/${userId}/name/update`, fullName, { headers: { Authorization: `Bearer ${token}` } });

            // Логируем ответ от сервера
            console.log('Ответ от сервера после сохранения:', response.data);

            // Обновляем состояние userData с новыми данными
            setUserData({ ...userData, ...formData });
            setIsEditing(false);
        } catch (error) {
            console.error('Ошибка при сохранении данных:', error.response ? error.response.data : error.message);
        }
    };

    const handleCancel = () => {
        setIsEditing(false);
        setFormData({
            first_name: userData.first_name,
            last_name: userData.last_name,
            middle_name: userData.middle_name
        });
    };

    if (loading) return <div>Загрузка...</div>;
    if (error) return <ErrorPage />;

    return (
        <div className="dashboard-container">
            <h2>Личный кабинет</h2>
            <div className="user-info">
                <p>
                    <strong>Имя: </strong>
                    {isEditing ? (
                        <input
                            type="text"
                            name="first_name"
                            value={formData.first_name}
                            onChange={handleChange}
                            className="input-field"
                        />
                    ) : (
                        <span>{userData.first_name}</span>
                    )}
                </p>
                <p>
                    <strong>Фамилия: </strong>
                    {isEditing ? (
                        <input
                            type="text"
                            name="last_name"
                            value={formData.last_name}
                            onChange={handleChange}
                            className="input-field"
                        />
                    ) : (
                        <span>{userData.last_name}</span>
                    )}
                </p>
                <p>
                    <strong>Отчество: </strong>
                    {isEditing ? (
                        <input
                            type="text"
                            name="middle_name"
                            value={formData.middle_name}
                            onChange={handleChange}
                            className="input-field"
                        />
                    ) : (
                        <span>{userData.middle_name}</span>
                    )}
                </p>
            </div>
            {isEditing ? (
                <div className="button-group">
                    <button className="logout-button" onClick={handleSave}>Сохранить</button>
                    <button className="logout-button" onClick={handleCancel}>Назад</button>
                </div>
            ) : (
                <button className="logout-button" onClick={handleEdit}>Изменить</button>
            )}

            <h3>Доступные дисциплины</h3>
            {userData.role === 'Студент' ? (
                <ul>
                    {disciplines.map((discipline) => (
                        <li key={discipline.id}>
                            {discipline.name} - <button>Посмотреть тесты</button>
                        </li>
                    ))}
                </ul>
            ) : userData.role === 'Преподаватель' ? (
                <ul>
                    {disciplines.map((discipline) => (
                        <li key={discipline.id}>
                            {discipline.name} - <button>Посмотреть студентов</button>
                        </li>
                    ))}
                </ul>
            ) : (
                <p>Нет доступных дисциплин для отображения.</p>
            )}

            <button className="logout-button" onClick={() => navigate('/logout')}>Выйти</button>
        </div>
    );
};

export default Dashboard;