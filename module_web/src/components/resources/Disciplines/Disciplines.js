// src/components/resources/Disciplines/Disciplines.js
import React, { useEffect, useState } from 'react';
import { Routes, Route, Link } from 'react-router-dom';
import axios from 'axios';
import DisciplineDetail from './DisciplineDetail';
import DisciplineEdit from './DisciplineEdit';
import DisciplineCreate from './DisciplineCreate';
import { fetchDisciplines } from './DisciplineAPI';

const Disciplines = () => {
    const [disciplines, setDisciplines] = useState([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        fetchDisciplinesData();
    }, []);

    const fetchDisciplinesData = async () => {
        setLoading(true);
        try {
            const response = await fetchDisciplines();
            setDisciplines(response.data);
        } catch (error) {
            console.error('Ошибка при загрузке списка дисциплин:', error);
        } finally {
            setLoading(false);
        }
    };

    if (loading) return <div>Загрузка...</div>;

    return (
        <Routes>
            <Route path="/" element={
                <div>
                    <h1>Список дисциплин</h1>
                    <ul>
                        {disciplines.map((discipline) => (
                            <li key={discipline.id}>
                                <Link to={`/disciplines/${discipline.id}`}>{discipline.name}</Link>
                            </li>
                        ))}
                    </ul>
                    <Link to="/disciplines/create">Создать дисциплину</Link>
                </div>
            } />
            <Route path="/:id" element={<DisciplineDetail />} />
            <Route path="/:id/edit" element={<DisciplineEdit />} />
            <Route path="/create" element={<DisciplineCreate />} />
        </Routes>
    );
};

export default Disciplines;