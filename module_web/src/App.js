import React, { useEffect, useState } from 'react';
import { BrowserRouter as Router, Route, Routes, Navigate } from 'react-router-dom';
import axios from 'axios';
import Home from './components/Home';
import Dashboard from './components/Dashboard';
import AuthCallback from './components/auth/AuthCallback';
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
import Login from './components/Login'; // Импортируем компонент Login

const App = () => {
    const [userStatus, setUserStatus] = useState('unknown'); // Начальное состояние - неизвестно
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        const checkSession = async () => {
            try {
                const response = await axios.get('http://localhost:5000/api/session', { withCredentials: true });
                setUserStatus(response.data.status); // 'authorized', 'anonymous', 'unknown'
            } catch (error) {
                console.error("Ошибка при проверке сессии:", error);
                setUserStatus('unknown');
            } finally {
                setLoading(false);
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
                <Route path="/" element={<Home userStatus={userStatus} />} />
                <Route path="/login" element={<Login userStatus={userStatus} setUserStatus={setUserStatus} />} />
                <Route path="/dashboard" element={userStatus === 'authorized' ? <Dashboard /> : <Navigate to="/" />} />
                <Route path="/logout" element={<Logout setUserStatus={setUserStatus} />} />
                <Route path="/auth/github/callback" element={<AuthCallback setUserStatus={setUserStatus} />} />
                <Route path="/unauthorized" element={<Unauthorized />} />
                <Route path="/forbidden" element={<Forbidden />} />
                <Route path="/error" element={<ErrorPage />} />

                <Route path="/users" element={userStatus === 'authorized' ? <Users /> : <Navigate to="/unauthorized" />} />
                <Route path="/disciplines" element={userStatus === 'authorized' ? <Disciplines /> : <Navigate to="/unauthorized" />} />
                <Route path="/tests" element={userStatus === 'authorized' ? <Tests /> : <Navigate to="/unauthorized" />} />
                <Route path="/questions" element={userStatus === 'authorized' ? <Questions /> : <Navigate to="/unauthorized" />} />
                <Route path="/attempts" element={userStatus === 'authorized' ? <Attempts /> : <Navigate to="/unauthorized" />} />
                <Route path="/answers" element={userStatus === 'authorized' ? <Answers /> : <Navigate to="/unauthorized" />} />

                <Route path="*" element={<Navigate to="/" />} />
            </Routes>
        </Router>
    );
};

export default App;