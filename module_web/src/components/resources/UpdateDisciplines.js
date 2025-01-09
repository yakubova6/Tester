import React, { useEffect, useState } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';
import '../styles/UpdateDisciplines.css'; // Убедитесь, что путь правильный

const UpdateDisciplines = () => {
    const [disciplines, setDisciplines] = useState([]);
    const [newDiscipline, setNewDiscipline] = useState('');
    const navigate = useNavigate();

    useEffect(() => {
        const fetchDisciplines = async () => {
            try {
                const response = await axios.get('/api/user/disciplines', { withCredentials: true });
                setDisciplines(response.data.disciplines || []);
            } catch (error) {
                console.error('Ошибка при загрузке дисциплин:', error);
            }
        };

        fetchDisciplines();
    }, []);

    const handleAddDiscipline = async () => {
        if (!newDiscipline.trim()) {
            alert('Пожалуйста, введите название дисциплины.');
            return;
        }
        try {
            const response = await axios.post('/api/user/disciplines', { name: newDiscipline }, { withCredentials: true });
            setDisciplines(prev => [...prev, response.data]);
            setNewDiscipline('');
        } catch (error) {
            console.error('Ошибка при добавлении дисциплины:', error.response?.data || error.message);
            alert('Ошибка при добавлении дисциплины: ' + (error.response?.data?.message || error.message)); // Более информативное сообщение
        }
    };

    const handleDeleteDiscipline = async (id) => {
        try {
            await axios.delete(`/api/user/disciplines/${id}`, { withCredentials: true });
            setDisciplines(prev => prev.filter(discipline => discipline.id !== id));
        } catch (error) {
            console.error('Ошибка при удалении дисциплины:', error);
        }
    };

    return (
        <div className="update-disciplines-container">
            <h2>Изменить дисциплины</h2>
            <input 
                type="text" 
                value={newDiscipline} 
                onChange={(e) => setNewDiscipline(e.target.value)} 
                className="input-class" 
                placeholder="Новая дисциплина" 
            />
            <button onClick={handleAddDiscipline} className="logout-button">Добавить дисциплину</button> {/* Изменяем стиль кнопки */}
            <ul>
                {disciplines.map(discipline => (
                    <li key={discipline.id}>
                        {discipline.name}
                        <button onClick={() => handleDeleteDiscipline(discipline.id)} className="login-button">Удалить</button>
                    </li>
                ))}
            </ul>
            <button onClick={() => navigate('/dashboard')} className="login-button">Назад</button>
        </div>
    );
};

export default UpdateDisciplines;