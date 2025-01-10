// src/components/resources/Disciplines/DisciplineCreate.js
import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { createDiscipline } from './DisciplineAPI';

const DisciplineCreate = () => {
    const navigate = useNavigate();
    const [name, setName] = useState('');
    const [description, setDescription] = useState('');

    const handleCreate = async () => {
        try {
            await createDiscipline({ name, description });
            navigate('/disciplines');
        } catch (error) {
            console.error('Ошибка при создании дисциплины:', error);
        }
    };

    return (
        <div>
            <h1>Создание дисциплины</h1>
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
            <button onClick={handleCreate}>Создать</button>
        </div>
    );
};

export default DisciplineCreate;