// permissions.js
const permissions = {
    student: {
        'user:list:read': true,
        'user:data:read': true,  // Посмотреть информацию о пользователе (курсы, оценки, тесты)
        'user:roles:read': false, // Посмотреть информацию о пользователе (роли)
        'user:block:read': true,  // Посмотреть заблокирован ли пользователь (себя)
        'course:user:add': false,  // Записать пользователя на дисциплину (других)
        'course:user:del': false,  // Отчислить пользователя с дисциплины (других)
        'course:test:read': true,  // Посмотреть информацию о тесте
    },
    teacher: {
        'user:list:read': true,
        'user:data:read': true,  // Посмотреть информацию о пользователе (курсы, оценки, тесты)
        'user:roles:read': true,  // Посмотреть информацию о пользователе (роли)
        'user:roles:write': false, // Изменить роли пользователя (другого)
        'user:block:read': true,   // Посмотреть заблокирован ли пользователь (себя)
        'course:test:read': true,  // Посмотреть тесты
        'course:test:write': true,  // Изменить тесты
        'course:user:add': false,   // Записать пользователей на дисциплину (других)
        'course:user:del': false,   // Отчислить пользователя с дисциплины (других)
        'course:info:write': true,   // Изменить информацию о дисциплине
        'course:test:list': true,     // Посмотреть информацию о дисциплине (Список тестов)
    },
    admin: {
        'user:list:read': true,
        'user:data:read': true,  // Посмотреть информацию о пользователе (курсы, оценки, тесты)
        'user:roles:read': true,  // Посмотреть информацию о пользователе (роли)
        'user:roles:write': true,  // Изменить роли пользователя
        'user:block:read': true,   // Посмотреть заблокирован ли пользователь
        'user:block:write': true,  // Заблокировать/Разблокировать пользователя
        'course:test:read': true,  // Посмотреть тесты
        'course:test:write': true,  // Изменить тесты
        'course:user:add': true,    // Записать пользователей на дисциплину
        'course:user:del': true,    // Отчислить пользователя с дисциплины
        'course:info:write': true,   // Изменить информацию о дисциплине
        'course:test:list': true,     // Посмотреть информацию о дисциплине (Список тестов)
        'course:add': true,           // Создать дисциплину
        'course:del': true,           // Удалить дисциплину
    }
};

module.exports = permissions;