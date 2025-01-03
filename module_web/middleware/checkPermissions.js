// middleware/checkPermissions.js
const permissions = require('../config/permissions');

const checkPermissions = (action) => {
    return (req, res, next) => {
        const user = req.user; // Предполагаем, что пользователь уже аутентифицирован и сохранён в req.user

        // Проверяем, аутентифицирован ли пользователь
        if (!user) {
            return res.status(401).json({ message: 'Неавторизованный доступ' });
        }

        // Проверяем, есть ли у пользователя роли
        const userRoles = user.roles || []; // Пытаемся получить роли пользователя, если их нет - используем пустой массив

        // Проверяем, имеет ли хоть одна роль пользователя доступ к действию
        const hasAccess = userRoles.some(role => permissions[role] && permissions[role][action] === true);

        if (!hasAccess) {
            return res.status(403).json({ message: 'Доступ запрещен' });
        }

        // Логирование для отладки
        console.log(`Пользователь ${user.id} имеет доступ к действию: ${action}`);

        next();
    };
};

module.exports = checkPermissions;