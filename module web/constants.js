// constants.js
const SECRET = '123'; // Секретный ключ для подписи JWT токенов

const USER_STATES = {
    UNKNOWN: 'Unknown',    // Пользователь не авторизован
    ANONYMOUS: 'Anonymous', // Пользователь начал процесс авторизации
    AUTHORIZED: 'Authorized' // Пользователь успешно авторизован
};

// Экспортируем константы для использования в других файлах
module.exports = { SECRET, USER_STATES };