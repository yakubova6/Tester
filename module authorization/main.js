const express = require("express");
const axios = require("axios");
const querystring = require("querystring");
const jwt = require("jsonwebtoken");
const { MongoClient } = require("mongodb");

const app = express();
const port = 3000;

const CLIENT_ID = "9bdb9a05b73646f580fe3b12e35b1825";  // Замените на ваш client_id Яндекса
const CLIENT_SECRET = "d7ee049d87e3461baadfd34deebd4ebc";  // Замените на ваш client_secret Яндекса
const REDIRECT_URI = "http://localhost:3000/callback";
const MONGO_URI = "mongodb://localhost:27017";  // Подключение к базе данных MongoDB

const authRequests = {};  // Словарь для хранения информации о запросах

const generateAuthorizationUrl = (token) => {
  return `https://oauth.yandex.ru/authorize?response_type=code&client_id=${CLIENT_ID}&state=${token}&redirect_uri=${encodeURIComponent(REDIRECT_URI)}`;
};

app.get("/authorize", (req, res) => {
  const { token } = req.query;

  if (!token) {
    return res.status(400).send("Ошибка: отсутствует токен входа");
  }

  const authUrl = generateAuthorizationUrl(token);

  const expirationTime = Date.now() + 5 * 60 * 1000;
  authRequests[token] = {
    expiresAt: expirationTime,
    status: "не получен"
  };

  res.send({ authUrl });
});

app.get("/callback", async (req, res) => {
  const { code, state } = req.query;

  if (!code || !state) {
    return res.status(400).send("Ошибка: код подтверждения или токен не найден");
  }

  const requestData = authRequests[state];
  if (!requestData || Date.now() > requestData.expiresAt) {
    return res.status(400).send("Ошибка: запрос устарел или не найден");
  }

  if (req.query.error) {
    requestData.status = "в доступе отказано";
    return res.send("Неудачная авторизация");
  }

  try {
    const tokenResponse = await axios.post(
      "https://oauth.yandex.ru/token",
      querystring.stringify({
        grant_type: "authorization_code",
        code,
        client_id: CLIENT_ID,
        client_secret: CLIENT_SECRET,
        redirect_uri: REDIRECT_URI
      }),
      { headers: { "Content-Type": "application/x-www-form-urlencoded" } }
    );

    const accessToken = tokenResponse.data.access_token;

    const userResponse = await axios.get("https://api.yandex.ru/v1/user", {
      headers: { Authorization: `Bearer ${accessToken}` }
    });

    const email = userResponse.data.email;

    const client = new MongoClient(MONGO_URI, { useNewUrlParser: true, useUnifiedTopology: true });
    await client.connect();
    const db = client.db("users_db");
    const usersCollection = db.collection("users");

    let user = await usersCollection.findOne({ email });
    if (!user) {
      const newUser = {
        name: `Аноним${Date.now()}`,
        role: "Студент",
        tokens: []
      };
      user = await usersCollection.insertOne(newUser);
    }

    const roles = ["role_based_permissions"];  // Здесь добавьте логику для ролей
    const accessTokenJWT = jwt.sign({ roles }, "access_secret", { expiresIn: "1m" });
    const refreshTokenJWT = jwt.sign({ email }, "refresh_secret", { expiresIn: "7d" });

    await usersCollection.updateOne({ _id: user._id }, { $push: { tokens: refreshTokenJWT } });

    requestData.status = "доступ предоставлен";
    requestData.access_token = accessTokenJWT;
    requestData.refresh_token = refreshTokenJWT;

    res.send("Успешная авторизация, можно вернуться в приложение!");
  } catch (error) {
    console.error("Ошибка получения данных:", error);
    res.status(500).send("Ошибка получения данных");
  }
});

app.listen(port, () => {
  console.log(`Authorization server запущен на http://localhost:${port}`);
});
