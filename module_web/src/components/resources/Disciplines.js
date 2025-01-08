import React, { useEffect, useState } from 'react';
import { Routes, Route, Link, useParams, useNavigate } from 'react-router-dom';
import axios from 'axios';

const Disciplines = () => {
    const [disciplines, setDisciplines] = useState([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        fetchDisciplines();
    }, []);

    const fetchDisciplines = () => {
        setLoading(true);
        axios
            .get('/api/disciplines')
            .then((response) => {
                setDisciplines(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке списка дисциплин:', error);
            })
            .finally(() => {
                setLoading(false);
            });
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

const DisciplineDetail = () => {
    const { id } = useParams();
    const [discipline, setDiscipline] = useState(null);
    const [tests, setTests] = useState([]);
    const [users, setUsers] = useState([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        setLoading(true);
        axios
            .get(`/api/disciplines/${id}`)
            .then((response) => {
                setDiscipline(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке данных дисциплины:', error);
            });

        axios
            .get(`/api/disciplines/${id}/tests`)
            .then((response) => {
                setTests(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке тестов дисциплины:', error);
            });

        axios
            .get(`/api/disciplines/${id}/users`)
            .then((response) => {
                setUsers(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке пользователей дисциплины:', error);
            })
            .finally(() => {
                setLoading(false);
            });
    }, [id]);

    const checkTestActiveStatus = async (testId) => {
        try {
            const response = await axios.get(`/api/disciplines/${id}/tests/${testId}/active`);
            return response.data.isActive;
        } catch (error) {
            console.error('Ошибка при проверке активности теста:', error);
            return false;
        }
    };

    const handleActivateTest = async (testId) => {
        try {
            await axios.put(`/api/disciplines/${id}/tests/${testId}/activate`);
            const isActive = await checkTestActiveStatus(testId);
            setTests((prevTests) =>
                prevTests.map((test) =>
                    test.id === testId ? { ...test, isActive } : test
                )
            );
        } catch (error) {
            console.error('Ошибка при активации теста:', error);
        }
    };

    const handleDeactivateTest = async (testId) => {
        try {
            await axios.put(`/api/disciplines/${id}/tests/${testId}/deactivate`);
            const isActive = await checkTestActiveStatus(testId);
            setTests((prevTests) =>
                prevTests.map((test) =>
                    test.id === testId ? { ...test, isActive } : test
                )
            );
        } catch (error) {
            console.error('Ошибка при деактивации теста:', error);
        }
    };

    const handleDeleteTest = (testId) => {
        axios
            .delete(`/api/disciplines/${id}/tests/${testId}`)
            .then(() => {
                setTests((prevTests) => prevTests.filter((test) => test.id !== testId));
            })
            .catch((error) => {
                console.error('Ошибка при удалении теста:', error);
            });
    };

    const handleEnrollUser = (userId) => {
        axios
            .put(`/api/disciplines/${id}/users/${userId}`)
            .then(() => {
                setUsers((prevUsers) =>
                    prevUsers.map((user) =>
                        user.id === userId ? { ...user, isEnrolled: true } : user
                    )
                );
            })
            .catch((error) => {
                console.error('Ошибка при записи пользователя:', error);
            });
    };

    const handleUnenrollUser = (userId) => {
        axios
            .delete(`/api/disciplines/${id}/users/${userId}`)
            .then(() => {
                setUsers((prevUsers) =>
                    prevUsers.map((user) =>
                        user.id === userId ? { ...user, isEnrolled: false } : user
                    )
                );
            })
            .catch((error) => {
                console.error('Ошибка при отчислении пользователя:', error);
            });
    };

    if (loading) return <div>Загрузка...</div>;

    return (
        <div>
            <h1>Детали дисциплины</h1>
            <p>Название: {discipline.name}</p>
            <p>Описание: {discipline.description}</p>
            <Link to={`/disciplines/${id}/edit`}>Редактировать</Link>

            <h2>Тесты дисциплины</h2>
            <ul>
                {tests.map((test) => (
                    <li key={test.id}>
                        <p>Название теста: {test.name}</p>
                        <p>Статус: {test.isActive ? 'Активен' : 'Неактивен'}</p>
                        <button onClick={() => handleActivateTest(test.id)}>Активировать</button>
                        <button onClick={() => handleDeactivateTest(test.id)}>Деактивировать</button>
                        <button onClick={() => handleDeleteTest(test.id)}>Удалить</button>
                    </li>
                ))}
            </ul>

            <h2>Пользователи дисциплины</h2>
            <ul>
                {users.map((user) => (
                    <li key={user.id}>
                        <p>ФИО: {user.fullName}</p>
                        <p>Статус: {user.isEnrolled ? 'Записан' : 'Не записан'}</p>
                        <button onClick={() => handleEnrollUser(user.id)}>Записать</button>
                        <button onClick={() => handleUnenrollUser(user.id)}>Отчислить</button>
                    </li>
                ))}
            </ul>
        </div>
    );
};

const DisciplineEdit = () => {
    const { id } = useParams();
    const navigate = useNavigate();
    const [name, setName] = useState('');
    const [description, setDescription] = useState('');
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        setLoading(true);
        axios
            .get(`/api/disciplines/${id}`)
            .then((response) => {
                setName(response.data.name);
                setDescription(response.data.description);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке данных дисциплины:', error);
            })
            .finally(() => {
                setLoading(false);
            });
    }, [id]);

    const handleSave = () => {
        axios
            .put(`/api/disciplines/${id}`, { name, description })
            .then(() => {
                navigate(`/disciplines/${id}`);
            })
            .catch((error) => {
                console.error('Ошибка при сохранении данных дисциплины:', error);
            });
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

const DisciplineCreate = () => {
    const navigate = useNavigate();
    const [name, setName] = useState('');
    const [description, setDescription] = useState('');

    const handleCreate = () => {
        axios
            .post('/api/disciplines', { name, description })
            .then(() => {
                navigate('/disciplines');
            })
            .catch((error) => {
                console.error('Ошибка при создании дисциплины:', error);
            });
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

export default Disciplines;