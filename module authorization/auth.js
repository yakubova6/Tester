const querystring = require('querystring');
const { MongoClient } = require('mongodb');

// .. MONGO
const uri = "mongodb://localhost:27017";
const dbName = "admin";
const collectionName = "users";

// .. YANDEX
const CLIENT_ID_YANDEX = '9bdb9a05b73646f580fe3b12e35b1825';
const CLIENT_SECRET_YANDEX = 'd7ee049d87e3461baadfd34deebd4ebc';
const REDIRECT_URI = 'http://localhost:5000/api/auth/callback';
const SCOPE = 'login:info login:email';

// .. GITHUB
const GITHUB_CLIENT_ID = 'Iv23liiWV4dof4W8N3G7';
const GITHUB_CLIENT_SECRET = '9870570d7f11e296b3dd633fef8b7abcb2cb663e';
const GITHUB_REDIRECT_URI = 'http://localhost:5000/api/auth/callback';
const GITHUB_AUTH_URL = 'https://github.com/login/oauth/authorize';
const GITHUB_TOKEN_URL = 'https://github.com/login/oauth/access_token';
const GITHUB_USER_URL = 'https://api.github.com/user';

exports.generateAuthYandexUrl = function (state) {
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
    const authUrl = `${GITHUB_AUTH_URL}?client_id=${GITHUB_CLIENT_ID}&redirect_uri=${GITHUB_REDIRECT_URI}&state=${state}&scope=user`;
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

exports.sendPostRequest = async function (name, idx, type) {
    const data = {
        name: name,
        idx: idx,
        type: type
    };

    const url = 'http://localhost:1111/api/db/addUser/';

    try {
        const response = await axios.post(url, data);
        console.log('Ответ от сервера:', response.data);
    } catch (error) {
        console.error('Ошибка при отправке запроса:', error.message);
    }
}