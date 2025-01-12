import React, { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { fetchDisciplineDetails, fetchUpdateDiscipline } from './DisciplineAPI';

const DisciplineEdit = ({ fetchDisciplinesData }) => { // Передаем функцию для обновления списка
    const { id } = useParams();
    const navigate = useNavigate();
    const [name, setName] = useState('');
    const [description, setDescription] = useState('');
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null); // Добавляем состояние для ошибок

    useEffect(() => {
        const fetchData = async () => {
            setLoading(true);
            try {
                const response = await fetchDisciplineDetails(id);
                setName(response.data.name);
                setDescription(response.data.description);
            } catch (error) {
                console.error('Ошибка при загрузке данных дисциплины:', error);
                setError('Не удалось загрузить данные дисциплины.'); // Устанавливаем сообщение об ошибке
            } finally {
                setLoading(false);
            }
        };

        fetchData();
    }, [id]);

    const handleSave = async () => {
        if (!name || !description) {
            setError('Заполните все поля'); // Проверка на пустые поля
            return;
        }

        try {
            await fetchUpdateDiscipline(id, { name, description }); // Используем правильную функцию
            await fetchDisciplinesData(); // Обновляем список дисциплин
            navigate(`/disciplines/${id}`); // Перенаправляем на страницу дисциплины
        } catch (error) {
            console.error('Ошибка при сохранении данных дисциплины:', error);
            setError('Не удалось сохранить данные дисциплины.'); // Устанавливаем сообщение об ошибке
        }
    };

    if (loading) return <div>Загрузка...</div>;

    return (
        <div>
            <h1>Редактирование дисциплины</h1>
            {error && <div style={{ color: 'red' }}>{error}</div>} {/* Отображаем сообщение об ошибке */}
            <div>
                <label>Название:</label>
                <input
                    type="text"
                    value={name}
                    onChange={(e) => setName(e.target.value)}
                />
            </div>
            <div>
                <label>Описание:</label>
                <textarea
                    value={description}
                    onChange={(e) => setDescription(e.target.value)}
                />
            </div>
            <button onClick={handleSave}>Сохранить</button>
        </div>
    );
};

export default DisciplineEdit;