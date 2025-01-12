import React, { useEffect, useState } from 'react';
import axios from 'axios';
import ErrorPage from './errors/ErrorPage';
import { useNavigate } from 'react-router-dom';
import './styles/Dashboard.css';
import Disciplines from './resources/Disciplines/Disciplines'; // Импортируйте компонент Disciplines

const Dashboard = () => {
    const [userData, setUserData] = useState({
        first_name: '',
        last_name: '',
        middle_name: '',
        roles: [] // Изменено на массив ролей
    });
    const [error, setError] = useState(false);
    const [loading, setLoading] = useState(true);
    const [isEditing, setIsEditing] = useState(false);
    const [formData, setFormData] = useState({
        first_name: '',
        last_name: '',
        middle_name: ''
    });
    const navigate = useNavigate();

    useEffect(() => {
        const checkSession = async () => {
            try {
                const sessionResponse = await axios.get('/api/session', { withCredentials: true });
                console.log('Проверка сессии:', sessionResponse.data);

                if (sessionResponse.data.status === 'authorized') {
                    const token = sessionResponse.data.token; 
                    const userId = sessionResponse.data.userId; 
                    await fetchUserData(userId, token); 
                } else {
                    navigate('/'); 
                }
            } catch (err) {
                console.error('Ошибка проверки сессии:', err);
                setError(true); 
            } finally {
                setLoading(false);
            }
        };

        const fetchUserData = async (userId, token) => {
            try {
                const userResponse = await axios.get(`/api/users`, { headers: { Authorization: `Bearer ${token}` } });
                const user = userResponse.data.users[0];

                if (user) {
                    setUserData({
                        id: user.id,
                        first_name: user.first_name,
                        last_name: user.last_name,
                        middle_name: user.middle_name,
                        roles: Array.isArray(user.roles) ? user.roles : [] // Проверка на массив
                    });
                    setFormData({
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

        checkSession();
    }, [navigate]);

    const handleChange = (e) => {
        const { name, value } = e.target;
        setFormData({ ...formData, [name]: value });
    };

    const handleEdit = () => {
        setIsEditing(true);
    };

    const handleSave = async () => {
        const sessionTokenRow = document.cookie.split('; ').find(row => row.startsWith('session_token'));
        const token = sessionTokenRow ? sessionTokenRow.split('=')[1] : null;

        try {
            const userId = userData.id; 
            if (!userId) {
                throw new Error('ID пользователя не найден'); 
            }

            const fullName = {
                first_name: formData.first_name,
                last_name: formData.last_name,
                middle_name: formData.middle_name
            };

            const response = await axios.put(`/api/users/${userId}/name/update`, fullName, { headers: { Authorization: `Bearer ${token}` } });
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

            <Disciplines userRole={userData.roles.includes('Teacher') ? 'Teacher' : 'Student'} userId={userData.id} />

            <button className="logout-button" onClick={() => navigate('/logout')}>Выйти</button>
        </div>
    );
};

export default Dashboard;