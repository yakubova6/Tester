import React, { useEffect, useState } from 'react';
import { BrowserRouter as Router, Route, Routes, Navigate } from 'react-router-dom';
import axios from 'axios';
import Dashboard from './components/Dashboard';
import AuthCallback from './components/auth/AuthCallback';
import AuthErrorPage from './components/errors/AuthErrorPage';
import Unauthorized from './components/errors/Unauthorized';
import Forbidden from './components/errors/Forbidden';
import ErrorPage from './components/errors/ErrorPage';
import Logout from './components/Logout';
import Users from './components/resources/Users';
import Disciplines from './components/resources/Disciplines';
import Tests from './components/resources/Tests';
import Questions from './components/resources/Questions';
import Attempts from './components/resources/Attempts';
import Answers from './components/resources/Answers';
import Login from './components/Login'; 
import UpdateDisciplines from './components/resources/UpdateDisciplines'; 
import CreateTest from './components/resources/CreateTest'; 

const App = () => {
    const [userStatus, setUserStatus] = useState('unknown');
    const [loading, setLoading] = useState(true);
    const [userData, setUserData] = useState(null);
    const [permissions, setPermissions] = useState([]);

    useEffect(() => {
        const checkSession = async () => {
            try {
                console.log("Проверка сессии..."); // Логируем начало проверки

                // Проверка наличия токена сессии в куках
                const response = await axios.get('/api/session', { withCredentials: true });
                console.log("Ответ от /api/session:", response.data); // Логируем ответ сервера

                setUserStatus(response.data.status); // Установка статуса пользователя

                if (response.data.status === 'authorized') {
                    const userDataResponse = await axios.get('/api/user-data', {
                        withCredentials: true // Обязательно добавьте это для передачи куков
                    });
                    console.log("Данные пользователя:", userDataResponse.data); // Логируем данные пользователя
                    setUserData(userDataResponse.data);
                    setPermissions(userDataResponse.data.permissions || []);
                }
            } catch (error) {
                console.error("Ошибка при проверке сессии:", error);

                if (error.response) {
                    // Ошибка получена от сервера
                    console.error("Статус ошибки:", error.response.status); // Логируем статус ошибки
                    console.error("Данные ошибки:", error.response.data); // Логируем данные ошибки

                    if (error.response.status === 401) {
                        console.log("Пользователь не авторизован (401)"); // Логируем конкретную ошибку
                        setUserStatus('unknown'); // Статус остается 'unknown'
                    } else {
                        console.log("Произошла другая ошибка, статус:", error.response.status);
                        setUserStatus('unknown');
                    }
                } else if (error.request) {
                    // Запрос был сделан, но ответа не было
                    console.error("Запрос был сделан, но ответа не было:", error.request);
                    setUserStatus('unknown'); // Статус остается 'unknown'
                } else {
                    // Произошла ошибка при настройке запроса
                    console.error("Ошибка при настройке запроса:", error.message);
                    setUserStatus('unknown'); // Статус остается 'unknown'
                }
            } finally {
                setLoading(false); // Завершаем загрузку
            }
        };

        checkSession();
    }, []);

    if (loading) {
        return <div>Загрузка...</div>;
    }

    return (
        <Router>
            <Routes>
                <Route path="/" element={<Login userStatus={userStatus} setUserStatus={setUserStatus} />} />
                <Route path="/login" element={<Navigate to="/" />} />
                <Route path="/dashboard" element={userStatus === 'authorized' ? <Dashboard userData={userData} permissions={permissions} /> : <Navigate to="/" />} />
                <Route path="/logout" element={<Logout setUserStatus={setUserStatus} />} />
                <Route path="/auth/callback" element={<AuthCallback setUserStatus={setUserStatus} />} />
                <Route path="/auth-error" element={<AuthErrorPage />} />
                <Route path="/unauthorized" element={<Unauthorized />} />
                <Route path="/forbidden" element={<Forbidden />} />
                <Route path="/error" element={<ErrorPage />} />
                <Route path="/users" element={userStatus === 'authorized' ? <Users permissions={permissions} /> : <Navigate to="/unauthorized" />} />
                <Route path="/disciplines" element={userStatus === 'authorized' ? <Disciplines permissions={permissions} /> : <Navigate to="/unauthorized" />} />
                <Route path="/tests" element={userStatus === 'authorized' ? <Tests permissions={permissions} /> : <Navigate to="/unauthorized" />} />
                <Route path="/questions" element={userStatus === 'authorized' ? <Questions permissions={permissions} /> : <Navigate to="/unauthorized" />} />
                <Route path="/attempts" element={userStatus === 'authorized' ? <Attempts permissions={permissions} /> : <Navigate to="/unauthorized" />} />
                <Route path="/answers" element={userStatus === 'authorized' ? <Answers permissions={permissions} /> : <Navigate to="/unauthorized" />} />
                <Route path="/update-disciplines" element={userStatus === 'authorized' ? <UpdateDisciplines /> : <Navigate to="/unauthorized" />} />
                <Route path="/create-test" element={userStatus === 'authorized' ? <CreateTest permissions={permissions} /> : <Navigate to="/unauthorized" />} />
                <Route path="*" element={<Navigate to="/" />} />
            </Routes>
        </Router>
    );
};

export default App;