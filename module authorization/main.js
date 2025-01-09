const express = require('express');
const axios = require('axios');
const querystring = require('querystring');
const { generateAuthYandexUrl } = require('./auth')

const CLIENT_ID = '9bdb9a05b73646f580fe3b12e35b1825';
const CLIENT_SECRET = 'd7ee049d87e3461baadfd34deebd4ebc';
const REDIRECT_URI = 'http://localhost:3000/callback';
const SCOPE = 'login:info login:email';

const app = express();
const port = 3000;

app.get('/authorize', (req, res) => {
    res.status(200).send(generateAuthYandexUrl());
});

app.get('/yesno', (req, res) => {
    const token = 'req.query.token';  

    if (!token) {
        return res.status(400).send('Token is missing');
    }

    const htmlContent = `
        <html>
        <body>
            <h2>Do you want to proceed?</h2>
            <button onclick="window.location.href='/answer?response=yes&token=${token}'">Yes</button>
            <button onclick="window.location.href='/answer?response=no&token=${token}'">No</button>
        </body>
        </html>
    `;

    res.send(htmlContent);
});

app.get('/answer', (req, res) => {
    const { response, token } = req.query;

    if (!token) {
        return res.status(400).send('Token is missing');
    }

    if (response === 'no') {
        return res.status(400).send('Access Denied');
    }

    if (response === 'yes') {
        const authUrl = generateAuthYandexUrl(); 
        return res.redirect(authUrl);  
    }

    return res.status(400).send('Invalid response');
});


app.get('/callback', async (req, res) => {
    const { code } = req.query;

    if (!code) {
        return res.status(400).send('Code is missing');
    }

    try {

        const response = await axios.post('https://oauth.yandex.ru/token', querystring.stringify({
            grant_type: 'authorization_code',
            code: code,
            client_id: CLIENT_ID,
            client_secret: CLIENT_SECRET,
            redirect_uri: REDIRECT_URI,
        }), {
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
        });

        const { access_token, refresh_token, expires_in, scope } = response.data;

        const userInfoResponse = await axios.get('https://login.yandex.ru/info', {
            headers: { Authorization: `OAuth ${access_token}` },
        });
        userInfo = userInfoResponse.data;
        console.log(userInfo)

        res.json({ access_token, refresh_token, expires_in, scope, });

    } catch (error) {
        res.status(500).send('Error exchanging code for token');
    }
});


app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});


// const express = require('express');
// const axios = require('axios');
// const querystring = require('querystring');

// const GITHUB_CLIENT_ID = 'Iv23liiWV4dof4W8N3G7';
// const GITHUB_CLIENT_SECRET = '9870570d7f11e296b3dd633fef8b7abcb2cb663e';
// const GITHUB_REDIRECT_URI = 'http://localhost:8000/callback';
// const GITHUB_AUTH_URL = 'https://github.com/login/oauth/authorize';
// const GITHUB_TOKEN_URL = 'https://github.com/login/oauth/access_token';
// const GITHUB_USER_URL = 'https://api.github.com/user';

// const app = express();
// const port = 8000;

// // Генерация URL для авторизации через GitHub
// app.get('/github/authorize', (req, res) => {
//     const authUrl = `${GITHUB_AUTH_URL}?client_id=${GITHUB_CLIENT_ID}&redirect_uri=${GITHUB_REDIRECT_URI}&scope=user`;
//     res.redirect(authUrl);
// });

// // Callback обработка для GitHub
// app.get('/callback', async (req, res) => {
//     console.log('success go to callback')
//     const { code } = req.query;

//     // Проверяем наличие кода
//     if (!code) {
//         return res.status(400).send('Code is missing');
//     }

//     try {
//         // Шаг 1: Обмен кода на токен
//         const tokenResponse = await axios.post(
//             GITHUB_TOKEN_URL,
//             querystring.stringify({
//                 client_id: GITHUB_CLIENT_ID,
//                 client_secret: GITHUB_CLIENT_SECRET,
//                 code,
//                 scope: 'user:email', // Запрашиваем доступ к email пользователя
//                 redirect_uri: GITHUB_REDIRECT_URI,
//             }),
//             {
//                 headers: {
//                     'Accept': 'application/json',
//                     'Content-Type': 'application/x-www-form-urlencoded',
//                 },
//             }
//         );
        
//         // Логируем токен для отладки
//         console.log('Token Response:', tokenResponse.data);

//         // Извлекаем access_token
//         const { access_token } = tokenResponse.data;

//         if (!access_token) {
//             return res.status(400).send('No access token received');
//         }

//         // Шаг 2: Получение информации о пользователе
//         const userInfoResponse = await axios.get(GITHUB_USER_URL, {
//             headers: { Authorization: `Bearer ${access_token}` },
//         });

//         const userInfo = userInfoResponse.data;

//         // Логируем информацию о пользователе
//         console.log('User Info:', userInfo.email);

//         // Шаг 3: Отправляем информацию о пользователе в ответ
//         res.json(userInfo);

//     } catch (error) {
//         // Логируем подробности ошибки
//         console.error('Error during token exchange or user info fetch:', error.response ? error.response.data : error.message);
//         res.status(500).send('Error during the process');
//     }
// });

// // Страница с кнопкой для авторизации через GitHub
// app.get('/', (req, res) => {
//     const htmlContent = `
//         <html>
//         <body>
//             <h2>GitHub Authorization</h2>
//             <a href="/github/authorize"><button>Login with GitHub</button></a>
//         </body>
//         </html>
//     `;
//     res.send(htmlContent);
// });

// app.listen(port, () => {
//     console.log(`Server running at http://localhost:${port}`);
// });
