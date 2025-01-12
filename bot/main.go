package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"os/signal"
	"strconv"
	"syscall"
	"time"

	"github.com/go-redis/redis/v8"
	tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
	"github.com/spf13/viper"

	"bot/front" 
)

var (
	rdb             *redis.Client
	registeredUsers = make(map[int64]bool) 
	bot             *tgbotapi.BotAPI       
)

// Инициализация конфигурации
func initConfig() {
	viper.SetConfigFile("config.yaml")
	if err := viper.ReadInConfig(); err != nil {
		log.Fatalf("Ошибка загрузки конфигурации: %s", err)
	}
}

// Инициализация Redis
func initRedis() {
	rdb = redis.NewClient(&redis.Options{
		Addr: viper.GetString("redis.address"), // Адрес Redis
	})

	_, err := rdb.Ping(rdb.Context()).Result()
	if err != nil {
		log.Fatalf("Ошибка подключения к Redis: %s", err)
	}
	log.Println("Подключение к Redis успешно")
}

// Получение ссылки для авторизации через GitHub
func getGitHubAuthURL(loginToken string) (string, error) {
	url := fmt.Sprintf("%s/github/getlink?loginToken=%s", viper.GetString("auth_module.url"), loginToken)
	resp, err := http.Get(url)
	if err != nil {
		return "", fmt.Errorf("ошибка при запросе URL GitHub: %v", err)
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return "", fmt.Errorf("ошибка при чтении ответа: %v", err)
	}

	var result map[string]string
	if err := json.Unmarshal(body, &result); err != nil {
		return "", fmt.Errorf("ошибка при разборе JSON: %v", err)
	}

	return result["authUrl"], nil
}

// Получение ссылки для авторизации через Яндекс
func getYandexAuthURL(loginToken string) (string, error) {
	url := fmt.Sprintf("%s/yandex/getlink?loginToken=%s", viper.GetString("auth_module.url"), loginToken)
	resp, err := http.Get(url)
	if err != nil {
		return "", fmt.Errorf("ошибка при запросе URL Яндекс: %v", err)
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return "", fmt.Errorf("ошибка при чтении ответа: %v", err)
	}

	var result map[string]string
	if err := json.Unmarshal(body, &result); err != nil {
		return "", fmt.Errorf("ошибка при разборе JSON: %v", err)
	}

	return result["authUrl"], nil
}

// Обмен кода на токены
func exchangeCodeForTokens(code, typeAuth, state string) (map[string]interface{}, error) {
	url := fmt.Sprintf("%s/api/auth/exchange", viper.GetString("auth_module.url"))

	// Логирование code, typeAuth и state
	log.Printf("Отправка запроса на обмен кода на токены: code=%s, type=%s, state=%s", code, typeAuth, state)

	// Тело запроса в формате JSON
	requestBody, err := json.Marshal(map[string]string{
		"code":  code,
		"type":  typeAuth,
		"state": state,
	})
	if err != nil {
		return nil, fmt.Errorf("ошибка при создании тела запроса: %v", err)
	}

	// Отправка POST-запроса
	resp, err := http.Post(url, "application/json", bytes.NewBuffer(requestBody))
	if err != nil {
		return nil, fmt.Errorf("ошибка при отправке запроса: %v", err)
	}
	defer resp.Body.Close()

	// Проверка статуса ответа
	if resp.StatusCode != http.StatusOK {
		body, _ := io.ReadAll(resp.Body)
		return nil, fmt.Errorf("сервер вернул статус %d: %s", resp.StatusCode, string(body))
	}

	// Чтение ответа
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, fmt.Errorf("ошибка при чтении ответа: %v", err)
	}

	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, fmt.Errorf("ошибка при разборе JSON: %v", err)
	}

	return result, nil
}

// Сохранение токенов в Redis
func saveTokensToRedis(chatID int64, accessToken, refreshToken string) error {
	data := map[string]string{
		"access_token":  accessToken,
		"refresh_token": refreshToken,
	}

	jsonData, err := json.Marshal(data)
	if err != nil {
		return fmt.Errorf("ошибка при сериализации данных: %v", err)
	}

	key := fmt.Sprintf("user:%d", chatID)
	if err := rdb.Set(rdb.Context(), key, jsonData, 0).Err(); err != nil {
		return fmt.Errorf("ошибка при сохранении в Redis: %v", err)
	}

	return nil
}

// Обработка команды /start
func handleStartCommand(chatID int64) {
	msg := tgbotapi.NewMessage(chatID, "Напишите /login для авторизации.")
	bot.Send(msg)
}

// Обработка команды /login
func handleLoginCommand(chatID int64) {
	msg := tgbotapi.NewMessage(chatID, "Выберите способ авторизации:")
	msg.ReplyMarkup = tgbotapi.NewInlineKeyboardMarkup(
		tgbotapi.NewInlineKeyboardRow(
			tgbotapi.NewInlineKeyboardButtonData("Войти через GitHub", "login_github"),
			tgbotapi.NewInlineKeyboardButtonData("Войти через Яндекс", "login_yandex"),
		),
	)
	bot.Send(msg)
}

// Обработка команды /logout
func handleLogoutCommand(chatID int64) {
	if _, exists := registeredUsers[chatID]; exists {
		delete(registeredUsers, chatID)
		msg := tgbotapi.NewMessage(chatID, "Вы успешно вышли из системы.")
		bot.Send(msg)
	} else {
		msg := tgbotapi.NewMessage(chatID, "Вы не зарегистрированы.")
		bot.Send(msg)
	}
}

// Обработка команды /status
func handleStatusCommand(chatID int64) {
	if _, exists := registeredUsers[chatID]; exists {
		msg := tgbotapi.NewMessage(chatID, "Вы зарегистрированы.")
		bot.Send(msg)
	} else {
		msg := tgbotapi.NewMessage(chatID, "Вы не зарегистрированы.")
		bot.Send(msg)
	}
}

// Обработка команды /create
func HandleCreateCommand(chatID int64) {
	// Проверка регистрации пользователя
	if _, exists := registeredUsers[chatID]; !exists {
		msg := tgbotapi.NewMessage(chatID, "Вы не зарегистрированы. Пожалуйста, зарегистрируйтесь через /login.")
		bot.Send(msg)
		return
	}

	log.Printf("Обработка команды /create для chatID: %d", chatID) // Логирование
	msg := tgbotapi.NewMessage(chatID, "Давайте приступим к созданию дисциплины.")

	// Создаем inline-кнопки
	inlineKeyboard := tgbotapi.NewInlineKeyboardMarkup(
		tgbotapi.NewInlineKeyboardRow(
			tgbotapi.NewInlineKeyboardButtonData("Создать дисциплину", "create_discipline"),
			tgbotapi.NewInlineKeyboardButtonData("Список дисциплин", "list_disciplines"),
		),
	)

	msg.ReplyMarkup = inlineKeyboard
	if _, err := bot.Send(msg); err != nil {
		log.Printf("Ошибка отправки сообщения: %v", err)
	}
}

// Обработка выбора способа авторизации
func handleLoginChoice(chatID int64, callbackQuery *tgbotapi.CallbackQuery) {
	// Генерация токена входа
	loginToken := fmt.Sprintf("login-token-%d", time.Now().Unix())

	// Логирование loginToken
	log.Printf("Сгенерирован loginToken: %s", loginToken)

	// Сохранение токена в Redis
	err := rdb.Set(rdb.Context(), loginToken, "anonymous", 5*time.Minute).Err()
	if err != nil {
		log.Printf("Ошибка сохранения токена в Redis: %s", err)
		msg := tgbotapi.NewMessage(chatID, "Ошибка авторизации. Попробуйте позже.")
		bot.Send(msg)
		return
	}

	// Определение типа авторизации
	var authURL string
	var errMsg error
	switch callbackQuery.Data {
	case "login_github":
		authURL, errMsg = getGitHubAuthURL(loginToken)
	case "login_yandex":
		authURL, errMsg = getYandexAuthURL(loginToken)
	default:
		msg := tgbotapi.NewMessage(chatID, "Неизвестный способ авторизации.")
		bot.Send(msg)
		return
	}

	if errMsg != nil {
		log.Printf("Ошибка получения URL для авторизации: %s", errMsg)
		msg := tgbotapi.NewMessage(chatID, "Ошибка авторизации. Попробуйте позже.")
		bot.Send(msg)
		return
	}
	log.Printf("Получена URL для авторизации: %s", authURL)

	// Регистрируем пользователя
	registeredUsers[chatID] = true

	// Отправка пользователю сообщения с ссылкой для авторизации
	msg := tgbotapi.NewMessage(chatID, fmt.Sprintf("Авторизируйтесь по ссылке: %s", authURL))
	bot.Send(msg)
}

// Обработчик для /api/auth/callback
func authCallbackHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/html; charset=utf-8")
	html := `
	<!DOCTYPE html>
	<html lang="ru">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Авторизация</title>
		<style>
			body {
				display: flex;
				justify-content: center;
				align-items: center;
				height: 100vh;
				margin: 0;
				font-family: Arial, sans-serif;
				background-color: #f0f0f0;
			}
			.message {
				font-size: 24px;
				color: #333;
			}
		</style>
	</head>
	<body>
		<div class="message">Вы успешно зарегистрированы!</div>
	</body>
	</html>
	`
	fmt.Fprint(w, html)

	// Получаем chatID из запроса (например, из query-параметра)
	chatIDStr := r.URL.Query().Get("chatID")
	if chatIDStr == "" {
		log.Println("ChatID не передан в запросе")
		return
	}

	chatID, err := strconv.ParseInt(chatIDStr, 10, 64)
	if err != nil {
		log.Printf("Ошибка преобразования chatID: %v", err)
		return
	}

	// Отправляем сообщение в Telegram
	msg := tgbotapi.NewMessage(chatID, "Вы успешно зарегистрированы!")
	bot.Send(msg)
}

func main() {
	// Инициализация конфигурации
	initConfig()

	// Инициализация Redis
	initRedis()

	// Инициализация бота
	var err error
	bot, err = tgbotapi.NewBotAPI(viper.GetString("telegram.token"))
	if err != nil {
		log.Panic(err)
	}
	bot.Debug = false // Отключаем режим отладки
	log.Printf("Авторизован как %s", bot.Self.UserName)

	// Запуск HTTP-сервера для обработки /api/auth/callback
	go func() {
		http.HandleFunc("/api/auth/callback", authCallbackHandler)
		log.Println("HTTP-сервер запущен на порту 9999")
		if err := http.ListenAndServe(":9999", nil); err != nil {
			log.Fatalf("Ошибка запуска HTTP-сервера: %v", err)
		}
	}()

	// Канал для обновлений
	updates := bot.GetUpdatesChan(tgbotapi.NewUpdate(0))

	// Канал для graceful shutdown
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)

	// Обработка обновлений
	for {
		select {
		case update := <-updates:
			if update.Message != nil {
				// Обработка ввода пользователя
				front.HandleUserInput(bot, update.Message, rdb)

				// Обработка команды /start
				if update.Message.Command() == "start" {
					handleStartCommand(update.Message.Chat.ID)
				}
				// Обработка команды /login
				if update.Message.Command() == "login" {
					handleLoginCommand(update.Message.Chat.ID)
				}
				// Обработка команды /logout
				if update.Message.Command() == "logout" {
					handleLogoutCommand(update.Message.Chat.ID)
				}
				// Обработка команды /status
				if update.Message.Command() == "status" {
					handleStatusCommand(update.Message.Chat.ID)
				}
				// Обработка команды /create
				if update.Message.Command() == "create" {
					HandleCreateCommand(update.Message.Chat.ID)
				}
			} else if update.CallbackQuery != nil {
				// Обработка выбора способа авторизации
				if update.CallbackQuery.Data == "login_github" || update.CallbackQuery.Data == "login_yandex" {
					handleLoginChoice(update.CallbackQuery.Message.Chat.ID, update.CallbackQuery)
				} else {
					// Обработка callback от inline-кнопок из front
					front.HandleCallbackQuery(bot, update.CallbackQuery, rdb)
				}
			}

		case <-sigChan:
			log.Println("Получен сигнал завершения, завершаю работу...")
			return
		}
	}
}
