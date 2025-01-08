import React, { useEffect, useState } from 'react';
import { Routes, Route, Link, useParams, useNavigate } from 'react-router-dom';
import axios from 'axios';

const Users = () => {
    const [users, setUsers] = useState([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        fetchUsers();
    }, []);

    const fetchUsers = () => {
        setLoading(true);
        axios
            .get('/api/users')
            .then((response) => {
                setUsers(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке списка пользователей:', error);
            })
            .finally(() => {
                setLoading(false);
            });
    };

    if (loading) return <div>Загрузка...</div>;

    return (
        <Routes>
            <Route
                path="/"
                element={
                    <div>
                        <h1>Список пользователей</h1>
                        <ul>
                            {users.map((user) => (
                                <li key={user.id}>
                                    <Link to={`/users/${user.id}`}>{user.fullName}</Link>
                                </li>
                            ))}
                        </ul>
                    </div>
                }
            />
            <Route path="/:id" element={<UserDetail />} />
            <Route path="/:id/edit" element={<UserEdit />} />
        </Routes>
    );
};

const UserDetail = () => {
    const { id } = useParams();
    const [user, setUser] = useState(null);
    const [courses, setCourses] = useState([]);
    const [roles, setRoles] = useState([]);
    const [grades, setGrades] = useState([]); // Новое состояние для оценок
    const [tests, setTests] = useState([]); // Новое состояние для тестов
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        setLoading(true);

        // Загрузка основной информации о пользователе
        axios
            .get(`/api/users/${id}/name`)
            .then((response) => {
                setUser(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке данных пользователя:', error);
            });

        // Загрузка курсов пользователя
        axios
            .get(`/api/users/${id}/courses`)
            .then((response) => {
                setCourses(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке курсов пользователя:', error);
            });

        // Загрузка ролей пользователя
        axios
            .get(`/api/users/${id}/roles`)
            .then((response) => {
                setRoles(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке ролей пользователя:', error);
            });

        // Загрузка оценок пользователя
        axios
            .get(`/api/users/${id}/grades`)
            .then((response) => {
                setGrades(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке оценок пользователя:', error);
            });

        // Загрузка тестов пользователя
        axios
            .get(`/api/users/${id}/tests`)
            .then((response) => {
                setTests(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке тестов пользователя:', error);
            })
            .finally(() => {
                setLoading(false);
            });
    }, [id]);

    const handleBlock = () => {
        axios
            .put(`/api/users/${id}/block`)
            .then(() => {
                setUser((prevUser) => ({ ...prevUser, isBlocked: true }));
            })
            .catch((error) => {
                console.error('Ошибка при блокировке пользователя:', error);
            });
    };

    const handleUnblock = () => {
        axios
            .put(`/api/users/${id}/unblock`)
            .then(() => {
                setUser((prevUser) => ({ ...prevUser, isBlocked: false }));
            })
            .catch((error) => {
                console.error('Ошибка при разблокировке пользователя:', error);
            });
    };

    if (loading) return <div>Загрузка...</div>;

    return (
        <div>
            <h1>Информация о пользователе</h1>
            <p>ФИО: {user.fullName}</p>
            <p>ID: {user.id}</p>
            <p>Статус: {user.isBlocked ? 'Заблокирован' : 'Активен'}</p>

            <div>
                <h2>Роли пользователя</h2>
                <ul>
                    {roles.map((role) => (
                        <li key={role.id}>{role.name}</li>
                    ))}
                </ul>
            </div>

            <div>
                <h2>Курсы пользователя</h2>
                <ul>
                    {courses.map((course) => (
                        <li key={course.id}>
                            <p>Название курса: {course.name}</p>
                            <p>Оценка: {course.grade}</p>
                            <p>Тесты: {course.tests.join(', ')}</p>
                        </li>
                    ))}
                </ul>
            </div>

            <div>
                <h2>Оценки пользователя</h2>
                <ul>
                    {grades.map((grade) => (
                        <li key={grade.id}>
                            <p>Курс: {grade.courseId}</p>
                            <p>Оценка: {grade.value}</p>
                        </li>
                    ))}
                </ul>
            </div>

            <div>
                <h2>Тесты пользователя</h2>
                <ul>
                    {tests.map((test) => (
                        <li key={test.id}>
                            <p>Название теста: {test.name}</p>
                            <p>Статус: {test.status}</p>
                        </li>
                    ))}
                </ul>
            </div>

            <div>
                <h2>Блокировка пользователя</h2>
                {user.isBlocked ? (
                    <button onClick={handleUnblock}>Разблокировать</button>
                ) : (
                    <button onClick={handleBlock}>Заблокировать</button>
                )}
            </div>

            <Link to={`/users/${id}/edit`}>Редактировать</Link>
        </div>
    );
};

const UserEdit = () => {
    const { id } = useParams();
    const navigate = useNavigate();
    const [fullName, setFullName] = useState('');
    const [roles, setRoles] = useState([]);
    const [availableRoles, setAvailableRoles] = useState([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        setLoading(true);
        axios
            .get(`/api/users/${id}/name`)
            .then((response) => {
                setFullName(response.data.fullName);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке данных пользователя:', error);
            });

        axios
            .get(`/api/users/${id}/roles`)
            .then((response) => {
                setRoles(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке ролей пользователя:', error);
            });

        axios
            .get('/api/roles')
            .then((response) => {
                setAvailableRoles(response.data);
            })
            .catch((error) => {
                console.error('Ошибка при загрузке доступных ролей:', error);
            })
            .finally(() => {
                setLoading(false);
            });
    }, [id]);

    const handleSave = () => {
        axios
            .put(`/api/users/${id}/name`, { fullName })
            .then(() => {
                axios
                    .put(`/api/users/${id}/roles`, { roles })
                    .then(() => {
                        navigate(`/users/${id}/name`);
                    })
                    .catch((error) => {
                        console.error('Ошибка при сохранении ролей пользователя:', error);
                    });
            })
            .catch((error) => {
                console.error('Ошибка при сохранении данных пользователя:', error);
            });
    };

    if (loading) return <div>Загрузка...</div>;

    return (
        <div>
            <h1>Редактирование пользователя</h1>
            <div>
                <label>ФИО:</label>
                <input
                    type="text"
                    value={fullName}
                    onChange={(e) => setFullName(e.target.value)}
                />
            </div>
            <div>
                <label>Роли:</label>
                <select
                    multiple
                    value={roles}
                    onChange={(e) => setRoles(Array.from(e.target.selectedOptions, (option) => option.value))}
                >
                    {availableRoles.map((role) => (
                        <option key={role.id} value={role.id}>
                            {role.name}
                        </option>
                    ))}
                </select>
            </div>
            <button onClick={handleSave}>Сохранить</button>
        </div>
    );
};

export default Users;