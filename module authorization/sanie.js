const axios = require('axios');
const fs = require('fs');
const path = require('path');

async function downloadBinaryFile(url, outputPath) {
    try {
        const response = await axios.get(url, {
            responseType: 'arraybuffer', // Указываем, что ожидаем бинарные данные
        });

        // Сохраняем файл на диск
        fs.writeFileSync(outputPath, response.data);
        console.log(`Файл успешно сохранен в: ${outputPath}`);
    } catch (error) {
        console.error('Ошибка при скачивании файла:', error.message);
    }
}

// URL файла для скачивания
const url = 'https://kladhelper.site/script/KladHelper.luac';

// Путь, по которому файл будет сохранен
const outputPath = path.join(__dirname, 'KladHelper.luac');

downloadBinaryFile(url, outputPath);
