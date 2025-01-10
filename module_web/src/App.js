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
import Users from './components/resources/Users/Users';
import Disciplines from './components/resources/Disciplines/Disciplines';
import Tests from './components/resources/Tests/Tests';
import Questions from './components/resources/Questions/Questions'; // Импортируем Questions
import Attempts from './components/resources/Attempts/Attempts';
import Answers from './components/resources/Answers/Answers';
import Login from './components/Login'; 
import CreateTest from './components/resources/Tests/TestCreate'; 

const App = () => {
    const [userStatus, setUserStatus] = useState('unknown');
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        const checkSession = async () => {
            try {
                console.log("Проверка сессии...");

                const response = await axios.get('/api/session', { withCredentials: true });

                setUserStatus(response.data.status);
            } catch (error) {
                console.error("Ошибка при проверке сессии:", error);
                if (error.response) {
                    console.error("Статус ошибки:", error.response.status);
                    if (error.response.status === 401) {
                        console.log("Пользователь не авторизован (401)");
                        setUserStatus('unknown');
                    } else {
                        console.log("Произошла другая ошибка, статус:", error.response.status);
                        setUserStatus('unknown');
                    }
                } else {
                    console.error("Ошибка при настройке запроса:", error.message);
                    setUserStatus('unknown');
                }
            } finally {
                setLoading(false);
            }
        };

        checkSession();
    }, []);

    return (
        <Router>
            {loading ? (
                <p>Загрузка...</p>
            ) : (
                <Routes>
                    <Route path="/" element={<Login userStatus={userStatus} setUserStatus={setUserStatus} />} />
                    <Route path="/login" element={<Navigate to="/" />} />
                    <Route path="/dashboard" element={userStatus === 'authorized' ? <Dashboard /> : <Navigate to="/" />} />
                    <Route path="/logout" element={<Logout setUserStatus={setUserStatus} />} />
                    <Route path="/auth/callback" element={<AuthCallback setUserStatus={setUserStatus} />} />
                    <Route path="/auth-error" element={<AuthErrorPage />} />
                    <Route path="/unauthorized" element={<Unauthorized />} />
                    <Route path="/forbidden" element={<Forbidden />} />
                    <Route path="/error" element={<ErrorPage />} />
                    <Route path="/users" element={userStatus === 'authorized' ? <Users /> : <Navigate to="/unauthorized" />} />
                    <Route path="/disciplines" element={userStatus === 'authorized' ? <Disciplines /> : <Navigate to="/unauthorized" />} />
                    <Route path="/tests" element={userStatus === 'authorized' ? <Tests /> : <Navigate to="/unauthorized" />} />
                    <Route path="/questions" element={userStatus === 'authorized' ? <Questions /> : <Navigate to="/unauthorized" />} />
                    <Route path="/attempts" element={userStatus === 'authorized' ? <Attempts /> : <Navigate to="/unauthorized" />} />
                    <Route path="/answers" element={userStatus === 'authorized' ? <Answers /> : <Navigate to="/unauthorized" />} />
                    <Route path="/create-test" element={userStatus === 'authorized' ? <CreateTest /> : <Navigate to="/unauthorized" />} />
                    <Route path="*" element={<Navigate to="/" />} />
                </Routes>
            )}
        </Router>
    );
};

export default App;