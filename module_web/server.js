const express = require('express');
const axios = require('axios');
const redis = require('redis');
const cookieParser = require('cookie-parser');
const bodyParser = require('body-parser');
const cors = require('cors');
const jwt = require('jsonwebtoken');
const path = require('path');
require('dotenv').config();

const app = express();
const port = 5000;

// Middleware
app.use(cors({
    origin: 'http://localhost:3000',
    methods: ['GET', 'POST', 'PUT', 'DELETE'],
    credentials: true
}));
app.use(bodyParser.json());
app.use(cookieParser());

// Подключение к Redis
const redisClient = redis.createClient();

redisClient.on('error', (err) => {
    console.error('Redis error:', err);
});

redisClient.connect().then(() => {
    console.log('Connected to Redis');
}).catch((err) => {
    console.error('Failed to connect to Redis:', err);
});

// Убедитесь, что секретный ключ загружен из переменных окружения
const SECRET_KEY = process.env.SECRET_KEY;

if (!SECRET_KEY) {
    console.error('SECRET_KEY is not defined in the environment variables');
    process.exit(1); // Завершить процесс, если SECRET_KEY не задан
}

// Middleware для проверки JWT токена доступа
const verifyToken = (req, res, next) => {
    const token = req.cookies.session_token;
    console.log('Checking token:', token);

    if (!token) {
        console.log('No token found');
        return res.status(401).json({ error: 'Unauthorized' });
    }

    jwt.verify(token, SECRET_KEY, (err, decoded) => {
        if (err) {
            console.log('Token verification failed:', err);
            return res.status(401).json({ error: 'Unauthorized' });
        }
        req.user = decoded; // Сохраняем данные пользователя в объекте запроса
        next();
    });
};

// Обработка ошибок
const handleError = (res, error, defaultMessage = 'Internal Server Error') => {
    console.error(error);
    res.status(500).json({ error: defaultMessage });
};

// Функция для отправки запроса к главному модулю 
const sendRequestToDBModule = async (method, url, data = {}, token) => {
    try {
        const response = await axios({
            method,
            url: `http://localhost:1111${url}`, // URL главного модуля 
            headers: {
                Authorization: `Bearer ${token}`, // Передаем токен
            },
            data,
        });
        return response.data;
    } catch (error) {
        console.error('Ошибка при запросе к главному модулю :', error.response?.data || error.message);
        throw error;
    }
};

// API маршруты

// Аутентификация
app.post('/api/auth/callback', async (req, res) => {
    const { code, state } = req.body;

    if (!code || !state) {
        return res.status(400).json({ error: 'Missing required parameters' });
    }

    try {
        // Проверяем, что state существует в Redis
        const stateData = await redisClient.get(state);
        if (!stateData) {
            return res.status(400).json({ error: 'Invalid state parameter.' });
        }

        // Парсим данные из state
        const { provider } = JSON.parse(stateData);

        // Отправляем запрос в модуль авторизации
        const authResponse = await axios.post('http://localhost:8000/api/auth/exchange', {
            code,
            state: provider, // Передаем провайдера (github, yandex или code)
        });

        // Получаем ответ от модуля авторизации
        const { sessionToken, accessToken, userInfo } = authResponse.data;

        // Сохраняем данные в Redis
        await redisClient.set(sessionToken, JSON.stringify({ accessToken, userInfo, status: 'authorized' }), 'EX', 43200);

        // Устанавливаем куку и возвращаем ответ
        res.cookie('session_token', sessionToken, { httpOnly: true, secure: false, sameSite: 'Lax' });
        res.json({ sessionToken, status: 'authorized' });

    } catch (error) {
        console.error('Error in /api/auth/callback:', error.response?.data || error.message); // Логируем ошибку
        handleError(res, error);
    }
});

// Проверка сессии
app.get('/api/session', async (req, res) => {
    const sessionToken = req.cookies.session_token;
    console.log('Session token received:', sessionToken); // Логируем значение токена

    if (!sessionToken) {
        console.log('No session token found');
        return res.status(401).json({ status: 'unknown' });
    }

    try {
        const data = await redisClient.get(sessionToken);
        if (!data) {
            console.log('No data found in Redis for token:', sessionToken);
            return res.status(401).json({ status: 'unknown' });
        }

        const sessionData = JSON.parse(data);
        console.log('Session data:', sessionData);
        res.status(200).json({ status: sessionData.status });
    } catch (error) {
        console.error('Error in /api/session:', error);
        handleError(res, error);
    }
});

// Получение данных пользователя
app.get('/api/user-data', verifyToken, async (req, res) => {
    const accessToken = req.user.accessToken;

    try {
        const userDataResponse = await axios.get('https://api.github.com/user', {
            headers: { Authorization: `Bearer ${accessToken}` },
        });
        res.status(200).json(userDataResponse.data);
    } catch (error) {
        handleError(res, error);
    }
});

// Выход
app.post('/api/logout', async (req, res) => {
    const sessionToken = req.cookies.session_token;

    if (!sessionToken) {
        return res.status(400).json({ error: 'Session token is missing' });
    }

    try {
        // Удаление токена из Redis
        await redisClient.del(sessionToken);

        // Очистка куки session_token
        res.clearCookie('session_token', { path: '/' });

        // Возвращаем успешный ответ
        res.status(200).json({ message: 'Logged out successfully' });
    } catch (error) {
        console.error('Ошибка при выходе:', error);
        handleError(res, error);
    }
});

// Дисциплины 

// Получить список всех дисциплин
app.get('/api/disciplines', verifyToken, async (req, res) => {
    try {
        const token = req.cookies.session_token;
        const disciplines = await sendRequestToDBModule('GET', '/api/db/disciplines', {}, token);
        res.status(200).json(disciplines);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить информацию о конкретной дисциплине по её ID
app.get('/api/disciplines/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const discipline = await sendRequestToDBModule('GET', `/api/db/disciplines/${id}`, {}, token);
        res.status(200).json(discipline);
    } catch (error) {
        handleError(res, error);
    }
});

// Создать новую дисциплину
app.post('/api/disciplines', verifyToken, async (req, res) => {
    try {
        const { name, description } = req.body;
        const token = req.cookies.session_token;
        const newDiscipline = await sendRequestToDBModule('POST', '/api/db/disciplines', { name, description }, token);
        res.status(201).json(newDiscipline);
    } catch (error) {
        handleError(res, error);
    }
});

// Обновить информацию о дисциплине по её ID
app.put('/api/disciplines/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const { name, description } = req.body;
        const token = req.cookies.session_token;
        const updatedDiscipline = await sendRequestToDBModule('PUT', `/api/db/disciplines/${id}`, { name, description }, token);
        res.status(200).json(updatedDiscipline);
    } catch (error) {
        handleError(res, error);
    }
});

// Удалить дисциплину по её ID
app.delete('/api/disciplines/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        await sendRequestToDBModule('DELETE', `/api/db/disciplines/${id}`, {}, token);
        res.status(204).send();
    } catch (error) {
        handleError(res, error);
    }
});

// Получить тесты дисциплины
app.get('/api/disciplines/:id/tests', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const tests = await sendRequestToDBModule('GET', `/api/db/disciplines/${id}/tests`, {}, token);
        res.status(200).json(tests);
    } catch (error) {
        handleError(res, error);
    }
});

// Активировать тест
app.put('/api/disciplines/:id/tests/:testId/activate', verifyToken, async (req, res) => {
    try {
        const { id, testId } = req.params;
        const token = req.cookies.session_token;
        const updatedTest = await sendRequestToDBModule('PUT', `/api/db/disciplines/${id}/tests/${testId}/activate`, {}, token);
        res.status(200).json(updatedTest);
    } catch (error) {
        handleError(res, error);
    }
});

// Деактивировать тест
app.put('/api/disciplines/:id/tests/:testId/deactivate', verifyToken, async (req, res) => {
    try {
        const { id, testId } = req.params;
        const token = req.cookies.session_token;
        const updatedTest = await sendRequestToDBModule('PUT', `/api/db/disciplines/${id}/tests/${testId}/deactivate`, {}, token);
        res.status(200).json(updatedTest);
    } catch (error) {
        handleError(res, error);
    }
});

// Удалить тест из дисциплины
app.delete('/api/disciplines/:id/tests/:testId', verifyToken, async (req, res) => {
    try {
        const { id, testId } = req.params;
        const token = req.cookies.session_token;
        await sendRequestToDBModule('DELETE', `/api/db/disciplines/${id}/tests/${testId}`, {}, token);
        res.status(204).send();
    } catch (error) {
        handleError(res, error);
    }
});

// Получить информацию о том, активен ли тест
app.get('/api/disciplines/:id/tests/:testId/active', verifyToken, async (req, res) => {
    try {
        const { id, testId } = req.params;
        const token = req.cookies.session_token;
        const isActive = await sendRequestToDBModule('GET', `/api/db/disciplines/${id}/tests/${testId}/active`, {}, token);
        res.status(200).json({ isActive });
    } catch (error) {
        handleError(res, error);
    }
});

// Получить список пользователей дисциплины
app.get('/api/disciplines/:id/users', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const users = await sendRequestToDBModule('GET', `/api/db/disciplines/${id}/users`, {}, token);
        res.status(200).json(users);
    } catch (error) {
        handleError(res, error);
    }
});

// Записать пользователя на дисциплину
app.put('/api/disciplines/:id/users/:userId', verifyToken, async (req, res) => {
    try {
        const { id, userId } = req.params;
        const token = req.cookies.session_token;
        const updatedUser = await sendRequestToDBModule('PUT', `/api/db/disciplines/${id}/users/${userId}`, {}, token);
        res.status(200).json(updatedUser);
    } catch (error) {
        handleError(res, error);
    }
});

// Отчислить пользователя с дисциплины
app.delete('/api/disciplines/:id/users/:userId', verifyToken, async (req, res) => {
    try {
        const { id, userId } = req.params;
        const token = req.cookies.session_token;
        const updatedUser = await sendRequestToDBModule('DELETE', `/api/db/disciplines/${id}/users/${userId}`, {}, token);
        res.status(200).json(updatedUser);
    } catch (error) {
        handleError(res, error);
    }
});

// Тесты

// Получить список всех тестов
app.get('/api/tests', verifyToken, async (req, res) => {
    try {
        const token = req.cookies.session_token;
        const tests = await sendRequestToDBModule('GET', '/api/db/tests', {}, token);
        res.status(200).json(tests);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить информацию о конкретном тесте по его ID
app.get('/api/tests/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const test = await sendRequestToDBModule('GET', `/api/db/tests/${id}`, {}, token);
        res.status(200).json(test);
    } catch (error) {
        handleError(res, error);
    }
});

// Создать новый тест
app.post('/api/tests', verifyToken, async (req, res) => {
    try {
        const { name, description, disciplineId } = req.body;
        const token = req.cookies.session_token;
        const newTest = await sendRequestToDBModule('POST', '/api/db/tests', { name, description, disciplineId }, token);
        res.status(201).json(newTest);
    } catch (error) {
        handleError(res, error);
    }
});

// Обновить информацию о тесте по его ID
app.put('/api/tests/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const { name, description, disciplineId } = req.body;
        const token = req.cookies.session_token;
        const updatedTest = await sendRequestToDBModule('PUT', `/api/db/tests/${id}`, { name, description, disciplineId }, token);
        res.status(200).json(updatedTest);
    } catch (error) {
        handleError(res, error);
    }
});

// Удалить тест по его ID
app.delete('/api/tests/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        await sendRequestToDBModule('DELETE', `/api/db/tests/${id}`, {}, token);
        res.status(204).send();
    } catch (error) {
        handleError(res, error);
    }
});

// Вопросы

// Получить список всех вопросов
app.get('/api/questions', verifyToken, async (req, res) => {
    try {
        const token = req.cookies.session_token;
        const questions = await sendRequestToDBModule('GET', '/api/db/questions', {}, token);
        res.status(200).json(questions);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить информацию о конкретном вопросе по его ID
app.get('/api/questions/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const question = await sendRequestToDBModule('GET', `/api/db/questions/${id}`, {}, token);
        res.status(200).json(question);
    } catch (error) {
        handleError(res, error);
    }
});

// Изменить информацию о вопросе по его ID
app.put('/api/questions/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const { text, testId } = req.body;
        const token = req.cookies.session_token;
        const updatedQuestion = await sendRequestToDBModule('PUT', `/api/db/questions/${id}`, { text, testId }, token);
        res.status(200).json(updatedQuestion);
    } catch (error) {
        handleError(res, error);
    }
});

// Создать новый вопрос
app.post('/api/questions', verifyToken, async (req, res) => {
    try {
        const { text, testId } = req.body;
        const token = req.cookies.session_token;
        const newQuestion = await sendRequestToDBModule('POST', '/api/db/questions', { text, testId }, token);
        res.status(201).json(newQuestion);
    } catch (error) {
        handleError(res, error);
    }
});

// Удалить вопрос по его ID
app.delete('/api/questions/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        await sendRequestToDBModule('DELETE', `/api/db/questions/${id}`, {}, token);
        res.status(204).send();
    } catch (error) {
        handleError(res, error);
    }
});

// Попытки

// Получить список всех попыток
app.get('/api/attempts', verifyToken, async (req, res) => {
    try {
        const token = req.cookies.session_token;
        const attempts = await sendRequestToDBModule('GET', '/api/db/attempts', {}, token);
        res.status(200).json(attempts);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить информацию о конкретной попытке по её ID
app.get('/api/attempts/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const attempt = await sendRequestToDBModule('GET', `/api/db/attempts/${id}`, {}, token);
        res.status(200).json(attempt);
    } catch (error) {
        handleError(res, error);
    }
});

// Создать новую попытку
app.post('/api/attempts', verifyToken, async (req, res) => {
    try {
        const { userId, testId, status } = req.body;
        const token = req.cookies.session_token;
        const newAttempt = await sendRequestToDBModule('POST', '/api/db/attempts', { userId, testId, status }, token);
        res.status(201).json(newAttempt);
    } catch (error) {
        handleError(res, error);
    }
});

// Обновить информацию о попытке по её ID
app.put('/api/attempts/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const { userId, testId, status } = req.body;
        const token = req.cookies.session_token;
        const updatedAttempt = await sendRequestToDBModule('PUT', `/api/db/attempts/${id}`, { userId, testId, status }, token);
        res.status(200).json(updatedAttempt);
    } catch (error) {
        handleError(res, error);
    }
});

// Удалить попытку по её ID
app.delete('/api/attempts/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        await sendRequestToDBModule('DELETE', `/api/db/attempts/${id}`, {}, token);
        res.status(204).send();
    } catch (error) {
        handleError(res, error);
    }
});

// Ответы

// Получить список всех ответов
app.get('/api/answers', verifyToken, async (req, res) => {
    try {
        const token = req.cookies.session_token;
        const answers = await sendRequestToDBModule('GET', '/api/db/answers', {}, token);
        res.status(200).json(answers);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить информацию о конкретном ответе по его ID
app.get('/api/answers/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const answer = await sendRequestToDBModule('GET', `/api/db/answers/${id}`, {}, token);
        res.status(200).json(answer);
    } catch (error) {
        handleError(res, error);
    }
});

// Создать новый ответ
app.post('/api/answers', verifyToken, async (req, res) => {
    try {
        const { attemptId, questionId, value } = req.body;
        const token = req.cookies.session_token;
        const newAnswer = await sendRequestToDBModule('POST', '/api/db/answers', { attemptId, questionId, value }, token);
        res.status(201).json(newAnswer);
    } catch (error) {
        handleError(res, error);
    }
});

// Обновить информацию о ответе по его ID
app.put('/api/answers/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const { attemptId, questionId, value } = req.body;
        const token = req.cookies.session_token;
        const updatedAnswer = await sendRequestToDBModule('PUT', `/api/db/answers/${id}`, { attemptId, questionId, value }, token);
        res.status(200).json(updatedAnswer);
    } catch (error) {
        handleError(res, error);
    }
});

// Удалить ответ по его ID
app.delete('/api/answers/:id', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        await sendRequestToDBModule('DELETE', `/api/db/answers/${id}`, {}, token);
        res.status(204).send();
    } catch (error) {
        handleError(res, error);
    }
});

// Пользователи

// Получить список всех пользователей
app.get('/api/users', verifyToken, async (req, res) => {
    try {
        const token = req.cookies.session_token;
        const users = await sendRequestToDBModule('GET', '/api/db/users', {}, token);
        res.status(200).json(users);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить информацию о конкретном пользователе по его ID
app.get('/api/users/:id/name', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const user = await sendRequestToDBModule('GET', `/api/db/users/${id}/name`, {}, token);
        res.status(200).json(user);
    } catch (error) {
        handleError(res, error);
    }
});

// Изменить информацию о пользователе по его ID (например, ФИО)
app.put('/api/users/:id/name', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const { fullName } = req.body;
        const token = req.cookies.session_token;
        const updatedUser = await sendRequestToDBModule('PUT', `/api/db/users/${id}/name`, { fullName }, token);
        res.status(200).json(updatedUser);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить список курсов, на которые записан пользователь
app.get('/api/users/:id/courses', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const courses = await sendRequestToDBModule('GET', `/api/db/users/${id}/courses`, {}, token);
        res.status(200).json(courses);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить оценки пользователя по его ID
app.get('/api/users/:id/grades', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const grades = await sendRequestToDBModule('GET', `/api/db/users/${id}/grades`, {}, token);
        res.status(200).json(grades);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить тесты пользователя по его ID
app.get('/api/users/:id/tests', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const tests = await sendRequestToDBModule('GET', `/api/db/users/${id}/tests`, {}, token);
        res.status(200).json(tests);
    } catch (error) {
        handleError(res, error);
    }
});

// Получить список ролей пользователя
app.get('/api/users/:id/roles', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const roles = await sendRequestToDBModule('GET', `/api/db/users/${id}/roles`, {}, token);
        res.status(200).json(roles);
    } catch (error) {
        handleError(res, error);
    }
});

// Изменить роли пользователя
app.put('/api/users/:id/roles', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const { roles } = req.body;
        const token = req.cookies.session_token;
        const updatedRoles = await sendRequestToDBModule('PUT', `/api/db/users/${id}/roles`, { roles }, token);
        res.status(200).json(updatedRoles);
    } catch (error) {
        handleError(res, error);
    }
});

// Проверить, заблокирован ли пользователь
app.get('/api/users/:id/block', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const isBlocked = await sendRequestToDBModule('GET', `/api/db/users/${id}/block`, {}, token);
        res.status(200).json({ isBlocked });
    } catch (error) {
        handleError(res, error);
    }
});

// Заблокировать пользователя
app.put('/api/users/:id/block', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const updatedUser = await sendRequestToDBModule('PUT', `/api/db/users/${id}/block`, {}, token);
        res.status(200).json(updatedUser);
    } catch (error) {
        handleError(res, error);
    }
});

// Разблокировать пользователя
app.put('/api/users/:id/unblock', verifyToken, async (req, res) => {
    try {
        const { id } = req.params;
        const token = req.cookies.session_token;
        const updatedUser = await sendRequestToDBModule('PUT', `/api/db/users/${id}/unblock`, {}, token);
        res.status(200).json(updatedUser);
    } catch (error) {
        handleError(res, error);
    }
});

// Обслуживание статических файлов из папки build
app.use(express.static(path.join(__dirname, 'build')));

// Все маршруты, которые не совпадают с API, перенаправляют на index.html
app.get('*', (req, res) => {
    res.sendFile(path.join(__dirname, 'build', 'index.html'));
});

// Запуск сервера
app.listen(port, () => {
    console.log(`Сервер запущен. Перейдите по ссылке: http://localhost:${port}`);
});