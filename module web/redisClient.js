// redisClient.js
const redis = require('redis');

// Создаем клиент Redis
const redisClient = redis.createClient({
    url: 'redis://redis:6379'
});

// Обрабатываем ошибки подключения
redisClient.on('error', (err) => console.log('Redis Client Error', err));

// Подключаемся к Redis
redisClient.connect();

// Экспортируем клиент Redis для использования в других файлах
module.exports = redisClient;