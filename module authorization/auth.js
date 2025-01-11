const querystring = require('querystring');
const axios = require('axios');
const { MongoClient } = require('mongodb');

// .. MONGO
const uri = "mongodb://127.0.0.1:27017";
const dbName = "admin";
const collectionName = "users";

// .. YANDEX
const CLIENT_ID_YANDEX = '9bdb9a05b73646f580fe3b12e35b1825';
const CLIENT_SECRET_YANDEX = 'd7ee049d87e3461baadfd34deebd4ebc';
let REDIRECT_URI = 'http://localhost:9999/api/auth/callback';
const SCOPE = 'login:info login:email';

// .. GITHUB
const GITHUB_CLIENT_ID = 'Iv23liiWV4dof4W8N3G7';
const GITHUB_CLIENT_SECRET = '9870570d7f11e296b3dd633fef8b7abcb2cb663e';
const GITHUB_REDIRECT_URI = '/api/auth/callback';
const GITHUB_AUTH_URL = 'https://github.com/login/oauth/authorize';
const GITHUB_TOKEN_URL = 'https://github.com/login/oauth/access_token';
const GITHUB_USER_URL = 'https://api.github.com/user';

exports.generateAuthYandexUrl = function (state, typeReq) {
    if (typeReq == 'web') {
        REDIRECT_URI = 'http://localhost:5000/api/auth/callback';
    } else {
        REDIRECT_URI = 'http://localhost:9999/api/auth/callback';
    }

    const authUrl = `https://oauth.yandex.ru/authorize?` +
        querystring.stringify({
            response_type: 'code',
            client_id: CLIENT_ID_YANDEX,
            redirect_uri: REDIRECT_URI,
            scope: SCOPE,
            state: state
        });
    return (JSON.stringify({ authUrl: authUrl }));
}

exports.generateAuthGithubUrl = function (state) {
    const authUrl = `${GITHUB_AUTH_URL}?client_id=${GITHUB_CLIENT_ID}&state=${state}&scope=user`;
    return (JSON.stringify({ authUrl: authUrl }));
}

exports.addOrUpdateUser = async function (email) {
    const client = new MongoClient(uri);
    let newUser = false;

    try {
        await client.connect();
        const database = client.db(dbName);
        const collection = database.collection(collectionName);

        let user = await collection.findOne({ email: email });

        if (user) {
            return user;
        } else {
            newUser = true;
            const userCount = await collection.countDocuments({});
            const userName = `Anonim ${userCount + 1}`;
            const newUserData = {
                email: email,
                name: userName,
                roles: ["Student"],
                tokens: []
            };

            const result = await collection.insertOne(newUserData);
            return newUserData;
        }
    } catch (error) {
        console.error("Ошибка:", error);
        return null;
    } finally {
        await client.close();
    }
};

exports.getUserIndexByEmail = async function (email) {
    const client = new MongoClient(uri);

    try {
        await client.connect();
        const database = client.db(dbName);
        const collection = database.collection(collectionName);

        // Получаем всех пользователей, отсортированных по email
        const allUsers = await collection.find({}).sort({ email: 1 }).toArray();

        // Находим индекс пользователя с заданным email
        const index = allUsers.findIndex(user => user.email === email);

        if (index !== -1) {
            return index + 1; // Индексы начинаются с 0, поэтому добавляем 1
        } else {
            console.log("Пользователь с таким email не найден.");
            return null;
        }
    } catch (error) {
        console.error("Ошибка:", error);
        return null;
    } finally {
        await client.close();
    }
};

exports.addTokenToUser = async function (email, refreshToken) {
    const client = new MongoClient(uri);

    try {
        await client.connect();
        const database = client.db(dbName);
        const collection = database.collection(collectionName);

        const user = await collection.findOne({ email: email });

        if (user) {
            if (!user.tokens.includes(refreshToken)) {
                user.tokens.push(refreshToken);
                await collection.updateOne(
                    { email: email },
                    { $set: { tokens: user.tokens } }
                );
                console.log(`Токен обновления для ${email} успешно добавлен.`);
            } else {
                console.log(`Токен для ${email} уже существует в списке.`);
            }
        } else {
            console.log(`Пользователь с email ${email} не найден.`);
        }
    } catch (error) {
        console.error("Ошибка:", error);
    } finally {
        await client.close();
    }
}

exports.getUserRoles = async function (email) {
    const client = new MongoClient(uri);
    try {
        await client.connect();
        const database = client.db(dbName);
        const collection = database.collection(collectionName);

        const user = await collection.findOne({ email: email });

        if (user) {
            return user.roles;
        } else {
            return null;
        }
    } catch (error) {
        console.error("Ошибка:", error);
        return null;
    } finally {
        await client.close();
    }
}

const permissions = {
    "Student": {
        "user": {
            "user:list:read": true,
            "user:fullName:read": true,
            "user:data:read": true,
            "user:block:read": true
        },
        "course": {
            "course:testList": true,
            "course:userList": true,
            "course:user:add": true,
            "course:user:del": true,
            "course:user:read": true
        },
        "test": {
            "test:answer:read": true,
            "test:answer:update": true,
            "test:answer:del": true
        },
        "quest": {
            "quest:list:read": true,
            "quest:read": true
        }
    },
    "Teacher": {
        "user": {
            "user:list:read": true,
            "user:fullName:read": true,
            "user:data:read": true,
            "user:roles:read": true,
            "user:block:read": true,
            "user:block:write": true
        },
        "course": {
            "course:add": true,
            "course:del": true,
            "course:testList": true,
            "course:test:read": true,
            "course:test:write": true,
            "course:userList": true,
            "course:user:add": true,
            "course:user:del": true
        },
        "test": {
            "test:quest:add": true,
            "test:quest:del": true,
            "test:quest:update": true,
            "test:answer:read": true,
            "test:answer:update": true,
            "test:answer:del": true
        },
        "quest": {
            "quest:create": true,
            "quest:update": true,
            "quest:del": true
        }
    },
    "Admin": {
        "user": {
            "user:list:read": true,
            "user:fullName:read": true,
            "user:data:read": true,
            "user:roles:read": true,
            "user:roles:write": true,
            "user:block:read": true,
            "user:block:write": true
        },
        "course": {
            "course:add": true,
            "course:del": true,
            "course:testList": true,
            "course:test:read": true,
            "course:test:write": true,
            "course:userList": true,
            "course:user:add": true,
            "course:user:del": true
        },
        "test": {
            "test:quest:add": true,
            "test:quest:del": true,
            "test:quest:update": true,
            "test:answer:read": true,
            "test:answer:update": true,
            "test:answer:del": true
        },
        "quest": {
            "quest:create": true,
            "quest:update": true,
            "quest:del": true
        }
    }
};

exports.getPermissionsByRoles = function (roles) {
    let allPermissions = [];

    roles.forEach(role => {
        const permissionsForRole = permissions[role];
        if (permissionsForRole) {
            allPermissions.push(permissionsForRole);
        }
    });

    return allPermissions;
}

exports.sendPostRequestMain = async function (idx) {
    const data = {
        uid: idx,
        type: 'addUser', // Фиксированное значение для type
    };

    const url = 'http://127.0.0.1:1111/api/db/addUser';

    try {
        const response = await axios.post(url, data, {
            headers: {
                'Content-Type': 'application/json',
            },
        });
        console.log('Ответ от сервера:', response.data);
    } catch (error) {
        if (error.response) {
            console.error('Ошибка ответа сервера:', error.response.data);
        } else {
            console.error('Ошибка при отправке запроса:', error.message);
        }
    }
};

const codesStorage = new Map();
const tokenStates = new Map();

exports.generateAuthCode = function (loginToken) {
    const code = generateRandomCode();
    const expirationTime = Date.now() + 1 * 60 * 1000; // Код устаревает через 1 минуту

    codesStorage.set(code, { loginToken, expirationTime });
    return code;
};

exports.verifyAuthCode = function (code, refreshToken) {
    const codeEntry = codesStorage.get(code);

    if (!codeEntry || codeEntry.expirationTime < Date.now()) {
        return { error: 'Код отсутствует или устарел.' };
    }

    const userData = jwt.verify(refreshToken, SECRET_KEY, (err, decoded) => {
        if (err) return null;
        return decoded;
    });

    if (!userData) {
        return { error: 'Неверный токен обновления.' };
    }

    codesStorage.delete(code); 
    return { email: userData.email, state: codeEntry.loginToken };
};

exports.addTokenState = function (loginToken, expirationTime = 5 * 60 * 1000) {
    tokenStates.set(loginToken, {
        expiresAt: Date.now() + expirationTime,
        status: 'pending',
    });
};

exports.updateTokenState = function (loginToken, status) {
    if (tokenStates.has(loginToken)) {
        const state = tokenStates.get(loginToken);
        state.status = status;
        tokenStates.set(loginToken, state);
    }
};