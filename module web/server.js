// server.js
const express = require('express');
const cookieParser = require('cookie-parser');
const authRoutes = require('./routes/authRoutes');
const mainRoutes = require('./routes/mainRoutes');

const app = express();
const port = 3000;

// Используем middleware для работы с JSON и куками
app.use(express.json());
app.use(cookieParser());

// Подключаем маршруты
app.use('/', mainRoutes); // Главные маршруты
app.use('/', authRoutes); // Маршруты для авторизации

// Запускаем сервер
app.listen(port, () => {
    console.log(`Web Client running on http://localhost:${port}`);
});