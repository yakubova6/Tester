const express = require('express');
const axios = require('axios');
const redis = require('redis');
const cookieParser = require('cookie-parser');
const bodyParser = require('body-parser');
const cors = require('cors');
const jwt = require('jsonwebtoken');
const path = require('path');

const app = express();
const port = 5000;

// Middleware
app.use(cors({
    origin: 'http://localhost:3000', 
    methods: ['GET', 'POST'], 
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

// Секретный ключ для подписи токенов
const SECRET_KEY = 'your-secret-key'; // Замените на безопасный ключ

// API маршруты

app.post('/api/auth/callback', async (req, res) => {
    console.log('Полученные параметры на сервере:', req.body);
    const { code, state } = req.body;

    // Проверьте, что код и состояние не равны null
    if (!code || !state) {
        console.error('Отсутствуют параметры code или state');
        return res.status(400).json({ error: 'Missing required parameters' });
    }

    try {
        // Обмен кода на токен доступа
        const response = await axios.post('https://github.com/login/oauth/access_token', {
            client_id: 'Ov23libRIJj8xTzQLJsw', // Ваш clientId
            client_secret: '03808686e40b18279bc7f37b9bdf57c1335625a2', // Ваш clientSecret
            code: code,
            state: state,
        }, { headers: { Accept: 'application/json' } });

        const accessToken = response.data.access_token;
        const generatedToken = jwt.sign({ accessToken }, SECRET_KEY, { expiresIn: '1h' });

        // Сохраните токен в Redis
        await redisClient.set(generatedToken, JSON.stringify({ accessToken }), 'EX', 3600); // 1 час
        console.log('Кука session_token установлена:', generatedToken);

        res.cookie('session_token', generatedToken, { httpOnly: true, secure: process.env.NODE_ENV === 'production' });
        console.log('Кука session_token установлена:', generatedToken);
        res.json({ sessionToken: generatedToken, status: 'authorized' });
    } catch (error) {
        console.error('Ошибка при обработке запроса:', error);
        if (error.response) {
            console.error('Статус ответа:', error.response.status);
            console.error('Данные ошибки:', error.response.data);
        }
        res.status(500).json({ error: 'Internal Server Error' });
    }
});

// Обработка запроса на выход
app.post('/api/logout', async (req, res) => {
    const sessionToken = req.cookies.session_token;

    if (!sessionToken) {
        console.error('Токен сессии отсутствует при выходе');
        return res.status(400).json({ error: 'Session token is missing' });
    }

    try {
        // Удаляем сессию из Redis
        await redisClient.del(sessionToken);
        res.clearCookie('session_token');
        console.log('Пользователь вышел из системы');
        res.status(200).json({ message: 'Logged out successfully' });
    } catch (error) {
        console.error('Logout error:', error);
        res.status(500).json({ error: 'Internal Server Error' });
    }
});

app.get('/api/user-data', async (req, res) => {
    const sessionToken = req.cookies.session_token;

    if (!sessionToken) {
        console.error('Токен сессии отсутствует');
        return res.status(401).json({ error: 'Unauthorized' });
    }

    const data = await redisClient.get(sessionToken);
    if (!data) {
        console.error('Данные для токена сессии не найдены');
        return res.status(401).json({ error: 'Unauthorized' });
    }

    const sessionData = JSON.parse(data);
    console.log('sessionData:', sessionData); // Логируем данные сессии

    try {
        const userDataResponse = await axios.get('http://localhost:5001/api/user-data', {
            headers: { Authorization: `Bearer ${sessionData.accessToken}` },
        });
        res.status(200).json(userDataResponse.data);
    } catch (error) {
        console.error('Ошибка при получении данных пользователя:', error);
        if (error.response) {
            console.error('Статус ответа от API пользователя:', error.response.status);
            console.error('Данные ошибки:', error.response.data);
        }
        res.status(500).json({ error: 'Internal Server Error' });
    }
});

// Обработка запроса на проверку сессии
app.get('/api/session', async (req, res) => {
    const sessionToken = req.cookies.session_token;

    if (!sessionToken) {
        console.log('Токен сессии отсутствует');
        return res.status(401).json({ status: 'unknown' });
    }

    try {
        const data = await redisClient.get(sessionToken);
        if (!data) {
            console.log('Данные для токена сессии не найдены');
            return res.status(401).json({ status: 'unknown' });
        }

        const sessionData = JSON.parse(data);
        res.status(200).json({ status: 'authorized' });
    } catch (error) {
        console.error('Ошибка проверки сессии:', error);
        res.status(500).json({ error: 'Internal Server Error' });
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
    const serverUrl = `http://localhost:${port}`;
    console.log(`Сервер запущен. Перейдите по ссылке: ${serverUrl}`);
});