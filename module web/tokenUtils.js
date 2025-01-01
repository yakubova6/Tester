// tokenUtils.js
const jwt = require('jsonwebtoken');
const { SECRET } = require('./constants');

// Функция для создания токена сессии
function createSessionToken(status) {
    return jwt.sign({ status }, SECRET, { algorithm: 'HS256' });
}

// Функция для создания токена входа
function createLoginToken(sessionToken) {
    return jwt.sign({ sessionToken }, SECRET, { algorithm: 'HS256' });
}

// Функция для проверки токена
function verifyToken(token) {
    try {
        return jwt.verify(token, SECRET, { algorithms: ['HS256'] });
    } catch (error) {
        return null; // Если токен недействителен
    }
}

// Экспортируем функции для использования в других файлах
module.exports = { createSessionToken, createLoginToken, verifyToken };