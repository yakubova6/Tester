const express = require('express');
const { authorizeWithGitHub, authorizeWithYandex, saveSession, getSession, deleteSession, generateToken, verifyToken } = require('./auth');
const { v4: uuidv4 } = require('uuid');
const app = express();
const port = 3000;

// Middleware для парсинга JSON
app.use(express.json());

// Главная страница
app.get('/', (req, res) => {
  res.send('<h1>Welcome to the Authorization Module</h1>');
});

// Авторизация через GitHub
app.get('/auth/github', async (req, res) => {
  const { code, state } = req.query;
  try {
    const user = await authorizeWithGitHub(code, state);
    const sessionId = uuidv4();
    await saveSession(sessionId, user);

    const token = generateToken({ sessionId });

    res.json({ message: 'Authorization successful', token });
  } catch (error) {
    res.status(400).json({ error: 'GitHub authorization failed' });
  }
});

// Авторизация через Yandex
app.get('/auth/yandex', async (req, res) => {
  const { code, state } = req.query;
  try {
    const user = await authorizeWithYandex(code, state);
    const sessionId = uuidv4();
    await saveSession(sessionId, user);

    const token = generateToken({ sessionId });

    res.json({ message: 'Authorization successful', token });
  } catch (error) {
    res.status(400).json({ error: 'Yandex authorization failed' });
  }
});

// Получение информации о пользователе
app.get('/profile', async (req, res) => {
  const { token } = req.headers;

  try {
    const decoded = verifyToken(token);
    const sessionId = decoded.sessionId;

    const user = await getSession(sessionId);

    if (!user) {
      return res.status(401).json({ error: 'Session expired' });
    }

    res.json({ profile: user });
  } catch (error) {
    res.status(401).json({ error: 'Invalid token' });
  }
});

// Логика выхода из системы
app.post('/logout', async (req, res) => {
  const { token } = req.headers;

  try {
    const decoded = verifyToken(token);
    const sessionId = decoded.sessionId;

    await deleteSession(sessionId);

    res.json({ message: 'Logged out successfully' });
  } catch (error) {
    res.status(400).json({ error: 'Error logging out' });
  }
});

app.listen(port, () => {
  console.log(`Server is running on http://localhost:${port}`);
});
