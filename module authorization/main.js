const express = require('express');
const axios = require('axios');
const querystring = require('querystring');


const CLIENT_ID = '9bdb9a05b73646f580fe3b12e35b1825';
const CLIENT_SECRET = 'd7ee049d87e3461baadfd34deebd4ebc';
const REDIRECT_URI = 'http://localhost:3000/callback';
const SCOPE = 'login:info login:email';  

const app = express();
const port = 3000;


app.get('/authorize', (req, res) => {
  const authUrl = `https://oauth.yandex.ru/authorize?` +
    querystring.stringify({
      response_type: 'code',
      client_id: CLIENT_ID,
      redirect_uri: REDIRECT_URI,
      scope: SCOPE,
    });
  res.status(500).send(authUrl);  
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
    res.json({
      access_token,
      refresh_token,
      expires_in,
      scope,
    });
  } catch (error) {
    res.status(500).send('Error exchanging code for token');
  }
});


app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
