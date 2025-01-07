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

        res.json({ access_token, refresh_token, expires_in, scope, });

    } catch (error) {
        res.status(500).send('Error exchanging code for token');
    }
});


app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});