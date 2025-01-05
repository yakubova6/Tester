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

// API маршруты

// Обработка колбэка GitHub
app.post('/api/auth/callback', async (req, res) => {
    const { code, state } = req.body;

    if (!code || !state) {
        return res.status(400).json({ error: 'Missing required parameters' });
    }

    try {
        // Обмен кода на токен доступа
        const response = await axios.post('https://github.com/login/oauth/access_token', {
            client_id: process.env.GITHUB_CLIENT_ID,
            client_secret: process.env.GITHUB_CLIENT_SECRET,
            code,
            state,
        }, { headers: { Accept: 'application/json' } });

        const accessToken = response.data.access_token;
        if (!accessToken) {
            return res.status(400).json({ error: 'Access token not received from GitHub.' });
        }

        const generatedToken = jwt.sign({ accessToken }, SECRET_KEY, { expiresIn: '12h' });

        // Сохраните токен в Redis
        await redisClient.set(generatedToken, JSON.stringify({ accessToken, status: 'authorized' }), 'EX', 43200); // 12 часов
        console.log('Сохраняю в Redis:', generatedToken, { accessToken, status: 'authorized' });
        
        // Установка куки с токеном сессии
        res.cookie('session_token', generatedToken, { httpOnly: true, secure: false, sameSite: 'Lax' });
        console.log('Устанавливаю куку session_token:', generatedToken);
        res.json({ sessionToken: generatedToken, status: 'authorized' });
    } catch (error) {
        handleError(res, error);
    }
});

// Обработка запроса на вход
app.post('/api/login', async (req, res) => {
    const { type } = req.body; // 'github' or 'yandex'

    if (!type) {
        return res.status(400).json({ error: 'Missing type parameter' });
    }

    // Генерация токена сессии и токена входа
    const sessionToken = generateSessionToken(); // Ваша функция генерации токена
    const entryToken = generateEntryToken(); // Ваша функция генерации токена входа

    // Сохранение в Redis
    await redisClient.set(sessionToken, JSON.stringify({ status: 'anonymous', entryToken }), 'EX', 3600); // 1 час

    // Отправка запроса к модулю авторизации
    const authResponse = await requestAuthorizationModule(type, entryToken);

    if (authResponse.error) {
        return res.status(403).json({ error: authResponse.error });
    }

    res.cookie('session_token', sessionToken, { httpOnly: true });
    res.json({ message: 'Authorization initiated', sessionToken });
});

// Обработка запроса на выход
app.post('/api/logout', async (req, res) => {
    const sessionToken = req.cookies.session_token;

    if (!sessionToken) {
        return res.status(400).json({ error: 'Session token is missing' });
    }

    try {
        await redisClient.del(sessionToken); // Удаление токена из Redis
        res.clearCookie('session_token'); // Очистка куки
        res.status(200).json({ message: 'Logged out successfully' });
    } catch (error) {
        handleError(res, error);
    }
});

// Обработка запроса на проверку сессии
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
        handleError(res, error);
    }
});

// Пример защищенного маршрута, который требует токен доступа
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