// src/components/resources/Disciplines/DisciplineEdit.js
import React, { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { fetchDisciplineDetails, updateDiscipline } from './DisciplineAPI';

const DisciplineEdit = () => {
    const { id } = useParams();
    const navigate = useNavigate();
    const [name, setName] = useState('');
    const [description, setDescription] = useState('');
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        const fetchData = async () => {
            setLoading(true);
            try {
                const response = await fetchDisciplineDetails(id);
                setName(response.data.name);
                setDescription(response.data.description);
            } catch (error) {
                console.error('Ошибка при загрузке данных дисциплины:', error);
            } finally {
                setLoading(false);
            }
        };

        fetchData();
    }, [id]);

    const handleSave = async () => {
        try {
            await updateDiscipline(id, { name, description });
            navigate(`/disciplines/${id}`);
        } catch (error) {
            console.error('Ошибка при сохранении данных дисциплины:', error);
        }
    };

    if (loading) return <div>Загрузка...</div>;

    return (
        <div>
            <h1>Редактирование дисциплины</h1>
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