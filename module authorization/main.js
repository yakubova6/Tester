const express = require('express');
const axios = require('axios');
const jwt = require('jsonwebtoken');
const querystring = require('querystring');
const { getUserIndexByEmail, updateTokenState, addTokenState, generateAuthCode, verifyAuthCode, sendPostRequestMain, addTokenToUser, generateAuthGithubUrl, generateAuthYandexUrl, addOrUpdateUser, getUserRoles, getPermissionsByRoles } = require('./auth');

async function getUserIndex() {
    try {
        const idx = await getUserIndexByEmail('jajaja@ha.ru');
        console.log(idx);
    } catch (error) {
        console.error("Ошибка при получении индекса:", error);
    }
}

getUserIndex();
