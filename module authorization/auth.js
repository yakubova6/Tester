const axios = require('axios');

const requestAuthorizationLink = async (token) => {
    try {
        const response = await axios.get('http://localhost:3000/authorize', {
            params: {
                token: token
            }
        });

        console.log("Authorization URL:", response.data.authUrl);
    } catch (error) {
        console.error("Ошибка при запросе:", error.response ? error.response.data : error.message);
    }
};

// Пример вызова функции с произвольным токеном
requestAuthorizationLink('123');
