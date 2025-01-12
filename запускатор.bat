@echo off
setlocal

REM Получаем путь к директории, в которой находится этот .bat файл
set "SCRIPT_DIR=%~dp0"

REM Переходим в директорию, где находится .bat файл
cd /d "%SCRIPT_DIR%"

REM Запускаем основную программу
start "" "%SCRIPT_DIR%module main\build\Debug\main_server.exe"

REM Ждем 2 секунды (опционально, чтобы основная программа успела запуститься)
timeout /t 2 >nul

REM Открываем новый терминал и выполняем команду node servertest.js
start cmd /k "cd /d "%SCRIPT_DIR%module authorization" && node servertest.js"

REM Ждем еще 2 секунды (опционально)
timeout /t 2 >nul

REM Запускаем второй .bat файл
start "" "%SCRIPT_DIR%module_web\Redis-7.4.2-Windows-x64-cygwin-with-Service\start.bat"

REM Ждем еще 2 секунды (опционально)
timeout /t 2 >nul

REM Открываем новый терминал и выполняем команду node server.js
start cmd /k "cd /d "%SCRIPT_DIR%module_web" && node server.js"

endlocal