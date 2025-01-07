const axios = require('axios');
const querystring = require('querystring');


const CLIENT_ID = '9bdb9a05b73646f580fe3b12e35b1825';
const CLIENT_SECRET = 'd7ee049d87e3461baadfd34deebd4ebc';
const REDIRECT_URI = 'http://localhost:3000/callback';
const SCOPE = 'login:info login:email';

exports.generateAuthYandexUrl = function() {
    const authUrl = `https://oauth.yandex.ru/authorize?` +
        querystring.stringify({
            response_type: 'code',
            client_id: CLIENT_ID,
            redirect_uri: REDIRECT_URI,
            scope: SCOPE,
        });
    return authUrl;
}

