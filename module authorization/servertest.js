const express = require('express');
const axios = require('axios');
const jwt = require('jsonwebtoken');
const querystring = require('querystring');
const { getUserIndexByEmail, updateTokenState, addTokenState, generateAuthCode, verifyAuthCode, sendPostRequestMain, addTokenToUser, generateAuthGithubUrl, generateAuthYandexUrl, addOrUpdateUser, getUserRoles, getPermissionsByRoles } = require('./auth')

const SECRET_KEY = 'your_secret_key';

const CLIENT_ID_YAN = '9bdb9a05b73646f580fe3b12e35b1825';
const CLIENT_SECRET_YAN = 'd7ee049d87e3461baadfd34deebd4ebc';
const REDIRECT_URI = 'http://localhost:8080/callback';

const GITHUB_CLIENT_ID = 'Iv23liiWV4dof4W8N3G7';
const GITHUB_CLIENT_SECRET = '27a3e1b31f7b9b2b211ce83acc97e6fc820e649a';
const GITHUB_REDIRECT_URI = 'http://localhost:8000/callback';
const GITHUB_AUTH_URL = 'https://github.com/login/oauth/authorize';
const GITHUB_TOKEN_URL = 'https://github.com/login/oauth/access_token';
const GITHUB_USER_URL = 'https://api.github.com/user';

const app = express();
app.use(express.json());

function generateRandomCode() {
    return Math.floor(100000 + Math.random() * 900000).toString();
}

app.get('/github/getlink', (req, res) => {
    const loginToken = req.query.loginToken;
    res.send(generateAuthGithubUrl(loginToken));
});

app.get('/yandex/getlink', (req, res) => {
    const loginToken = req.query.loginToken;
    res.send(generateAuthYandexUrl(loginToken));
});

app.post('/api/auth/code/generate', (req, res) => {
    const { loginToken } = req.body;

    if (!loginToken) {
        return res.status(400).json({ error: 'Не передан токен входа.' });
    }

    const code =  generateRandomCode(loginToken);
    addTokenState(loginToken); 
    console.log(code)
    res.json({ code });
});


app.post('/api/auth/exchange', async (req, res) => {
    console.log(req.body)
    const { code, type, state } = req.body;
    if (!state || (!code)) {
        // return res.status(400).json({ error: 'Отсутствуют необходимые параметры: state, code или loginToken.' });
    }

    try {
        let userInfo;
        let accessToken;
        let permissionsForUser
        let userEmail

        if (type === 'github') {
            const response = await axios.post('https://github.com/login/oauth/access_token', {
                client_id: GITHUB_CLIENT_ID,
                client_secret: GITHUB_CLIENT_SECRET,
                code,
            }, { headers: { Accept: 'application/json' } });
            console.log(response.data)
            accessToken = response.data.access_token;
            if (!accessToken) {
                return res.status(400).json({ error: 'GitHub не предоставил токен доступа.' });
            }

            const userDataResponse = await axios.get('https://api.github.com/user', {
                headers: { Authorization: `Bearer ${accessToken}` },
            });

            userInfo = userDataResponse.data;
            userEmail = userInfo.email
            let roles = await getUserRoles(userEmail);

            if (roles) {
                permissionsForUser = getPermissionsByRoles(roles)
            } else {
                await addOrUpdateUser(userEmail);
                permissionsForUser = getPermissionsByRoles(['Student'])
            }
            

        } else if (type === 'yandex') {
            const response = await axios.post('https://oauth.yandex.ru/token', querystring.stringify({
                grant_type: 'authorization_code',
                code,
                client_id: CLIENT_ID_YAN,
                client_secret: CLIENT_SECRET_YAN,
                redirect_uri: REDIRECT_URI,
            }), {
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            });

            accessToken = response.data.access_token;
            if (!accessToken) {
                return res.status(400).json({ error: 'Yandex не предоставил токен доступа.' });
            }

            const userInfoResponse = await axios.get('https://login.yandex.ru/info', {
                headers: { Authorization: `OAuth ${accessToken}` },
            });

            userInfo = userInfoResponse.data;
            userEmail = userInfo.default_email
            let roles = await getUserRoles(userEmail);

            if (roles) {
                permissionsForUser = getPermissionsByRoles(roles)
            } else {
                await addOrUpdateUser(userEmail);
                permissionsForUser = getPermissionsByRoles(['Student'])
            }

        } else if (type === 'code') {
            const { error, email, state } = verifyAuthCode(code, state);
        
            if (error) {
                return res.status(400).json({ error });
            }
        
            const roles = await getUserRoles(email);
        
            if (roles) {
                permissionsForUser = getPermissionsByRoles(roles);
            } else {
                await addOrUpdateUser(email);
                permissionsForUser = getPermissionsByRoles(['Student']);
            }
        
            accessToken = jwt.sign({ permissions: permissionsForUser, userInfo: { email } }, SECRET_KEY, { expiresIn: '1m' });
            const refreshToken = jwt.sign({ email }, SECRET_KEY, { expiresIn: '7d' });
        
            addTokenToUser(email, refreshToken);
            const sessionToken = jwt.sign({ accessToken, email }, SECRET_KEY, { expiresIn: '12h' });
        
            res.json({ sessionToken, accessToken, refreshToken, email });
        } else {
            return res.status(400).json({ error: `Некорректный параметр state: ${state}` });
        }

        let userIdx = getUserIndexByEmail(userEmail)
        sendPostRequestMain(userIdx)
        
        accessToken = jwt.sign({ permissions: permissionsForUser, userInfo, userIdx }, SECRET_KEY, { expiresIn: '1m' });
        const refreshToken = jwt.sign({ email: userInfo.email }, SECRET_KEY, { expiresIn: '7d' });
        addTokenToUser(userEmail, refreshToken);

        const sessionToken = jwt.sign({ accessToken, userInfo, userIdx }, SECRET_KEY, { expiresIn: '12h' });
        res.json({ sessionToken, accessToken, refreshToken, userInfo });


    } catch (error) {
        console.error('Ошибка авторизации:', error.message);
        res.status(500).json({ error: 'Ошибка авторизации.' });
    }
});

const PORT = 8000;
app.listen(PORT, () => {
    console.log(`Auth server is running on port http://localhost:${PORT}`);
});