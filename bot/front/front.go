package front

import (
	"encoding/json"
	"fmt"
	"log"
	"strconv"
	"strings"

	"github.com/go-redis/redis/v8"
	tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
)

// Генерация ключа для хранения состояния пользователя в Redis
func getStateKey(chatID int64) string {
	return fmt.Sprintf("user:%d:state", chatID)
}

// Генерация ключа для хранения списка дисциплин пользователя в Redis
func getDisciplinesKey(chatID int64) string {
	return fmt.Sprintf("user:%d:disciplines", chatID)
}

// Генерация ключа для хранения списка тестов дисциплины в Redis
func getTestsKey(chatID int64, disciplineName string) string {
	return fmt.Sprintf("user:%d:%s:tests", chatID, disciplineName)
}

// Генерация ключа для хранения вопроса в Redis
func getQuestionKey(chatID int64, testName string) string {
	return fmt.Sprintf("user:%d:%s:question", chatID, testName)
}

// Обработка callback от inline-кнопок
func HandleCallbackQuery(bot *tgbotapi.BotAPI, callbackQuery *tgbotapi.CallbackQuery, rdb *redis.Client) {
	switch {
	case callbackQuery.Data == "create_discipline":
		chatID := callbackQuery.Message.Chat.ID
		err := rdb.Set(rdb.Context(), getStateKey(chatID), "creating_discipline", 0).Err()
		if err != nil {
			log.Printf("Ошибка сохранения состояния в Redis: %v", err)
			return
		}

		msg := tgbotapi.NewMessage(chatID, "Введите название новой дисциплины.")
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения: %v", err)
		}

	case callbackQuery.Data == "list_disciplines":
		chatID := callbackQuery.Message.Chat.ID
		disciplines, err := getDisciplines(rdb, chatID)
		if err != nil {
			log.Printf("Ошибка получения списка дисциплин: %v", err)
			msg := tgbotapi.NewMessage(chatID, "Ошибка при получении списка дисциплин.")
			bot.Send(msg)
			return
		}

		// Формируем inline-кнопки для каждой дисциплины
		var rows [][]tgbotapi.InlineKeyboardButton
		for _, discipline := range disciplines {
			row := tgbotapi.NewInlineKeyboardRow(
				tgbotapi.NewInlineKeyboardButtonData(discipline, "show_tests_"+discipline),
			)
			rows = append(rows, row)
		}

		msg := tgbotapi.NewMessage(chatID, "Выберите дисциплину, чтобы увидеть список тестов:")
		msg.ReplyMarkup = tgbotapi.NewInlineKeyboardMarkup(rows...)
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения: %v", err)
		}

	case strings.HasPrefix(callbackQuery.Data, "create_test_"):
		// Извлекаем название дисциплины из callback_data
		disciplineName := strings.TrimPrefix(callbackQuery.Data, "create_test_")

		chatID := callbackQuery.Message.Chat.ID
		err := rdb.Set(rdb.Context(), getStateKey(chatID), "creating_test_"+disciplineName, 0).Err()
		if err != nil {
			log.Printf("Ошибка сохранения состояния в Redis: %v", err)
			return
		}

		// Запрашиваем у пользователя название теста
		msg := tgbotapi.NewMessage(chatID, "Придумайте название теста и напишите его.")
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения: %v", err)
		}

	case strings.HasPrefix(callbackQuery.Data, "show_tests_"):
		// Извлекаем название дисциплины из callback_data
		disciplineName := strings.TrimPrefix(callbackQuery.Data, "show_tests_")

		// Получаем список тестов для выбранной дисциплины
		chatID := callbackQuery.Message.Chat.ID
		tests, err := getTests(rdb, chatID, disciplineName)
		if err != nil {
			log.Printf("Ошибка получения списка тестов: %v", err)
			msg := tgbotapi.NewMessage(chatID, "Ошибка при получении списка тестов.")
			bot.Send(msg)
			return
		}

		// Формируем inline-кнопки для каждого теста
		var rows [][]tgbotapi.InlineKeyboardButton
		for _, test := range tests {
			row := tgbotapi.NewInlineKeyboardRow(
				tgbotapi.NewInlineKeyboardButtonData(test, "select_test_"+test),
			)
			rows = append(rows, row)
		}

		// Добавляем кнопку для возврата к списку дисциплин
		rows = append(rows, tgbotapi.NewInlineKeyboardRow(
			tgbotapi.NewInlineKeyboardButtonData("Назад к дисциплинам", "list_disciplines"),
		))

		// Отправляем сообщение с inline-кнопками
		msg := tgbotapi.NewMessage(chatID, fmt.Sprintf("Список тестов для дисциплины «%s»:", disciplineName))
		msg.ReplyMarkup = tgbotapi.NewInlineKeyboardMarkup(rows...)
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения: %v", err)
		}

	case strings.HasPrefix(callbackQuery.Data, "select_test_"):
		// Извлекаем название теста из callback_data
		testName := strings.TrimPrefix(callbackQuery.Data, "select_test_")

		// Устанавливаем состояние "viewing_test" для пользователя
		chatID := callbackQuery.Message.Chat.ID
		err := rdb.Set(rdb.Context(), getStateKey(chatID), "viewing_test_"+testName, 0).Err()
		if err != nil {
			log.Printf("Ошибка сохранения состояния в Redis: %v", err)
			return
		}

		// Отправляем сообщение с действиями для теста
		msg := tgbotapi.NewMessage(chatID, fmt.Sprintf("Вы выбрали тест «%s». Что вы хотите сделать?", testName))
		msg.ReplyMarkup = tgbotapi.NewInlineKeyboardMarkup(
			tgbotapi.NewInlineKeyboardRow(
				tgbotapi.NewInlineKeyboardButtonData("Просмотреть вопросы", "view_questions_"+testName),
				tgbotapi.NewInlineKeyboardButtonData("Добавить вопрос", "create_question_"+testName),
			),
			tgbotapi.NewInlineKeyboardRow(
				tgbotapi.NewInlineKeyboardButtonData("Назад к тестам", "show_tests_"+testName),
			),
		)
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения: %v", err)
		}

	case strings.HasPrefix(callbackQuery.Data, "view_questions_"):
		// Извлекаем название теста из callback_data
		testName := strings.TrimPrefix(callbackQuery.Data, "view_questions_")

		// Получаем список вопросов для выбранного теста
		chatID := callbackQuery.Message.Chat.ID
		questionsKey := fmt.Sprintf("user:%d:%s:questions", chatID, testName)
		questions, err := rdb.LRange(rdb.Context(), questionsKey, 0, -1).Result()
		if err != nil {
			log.Printf("Ошибка получения списка вопросов: %v", err)
			msg := tgbotapi.NewMessage(chatID, "Ошибка при получении списка вопросов.")
			bot.Send(msg)
			return
		}

		// Формируем сообщение с списком вопросов
		messageText := fmt.Sprintf("Список вопросов для теста «%s»:\n", testName)
		if len(questions) == 0 {
			messageText += "Вопросов пока нет."
		} else {
			for i, question := range questions {
				var q map[string]interface{}
				if err := json.Unmarshal([]byte(question), &q); err != nil {
					log.Printf("Ошибка десериализации вопроса: %v", err)
					continue
				}
				messageText += fmt.Sprintf("%d. %s\n", i+1, q["question"])
			}
		}

		// Отправляем сообщение с списком вопросов
		msg := tgbotapi.NewMessage(chatID, messageText)
		msg.ReplyMarkup = tgbotapi.NewInlineKeyboardMarkup(
			tgbotapi.NewInlineKeyboardRow(
				tgbotapi.NewInlineKeyboardButtonData("Добавить вопрос", "create_question_"+testName),
				tgbotapi.NewInlineKeyboardButtonData("Назад к тесту", "select_test_"+testName),
			),
		)
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения: %v", err)
		}

	case strings.HasPrefix(callbackQuery.Data, "create_question_"):
		// Извлекаем название теста из callback_data
		testName := strings.TrimPrefix(callbackQuery.Data, "create_question_")

		// Устанавливаем состояние "creating_question" для пользователя
		chatID := callbackQuery.Message.Chat.ID
		err := rdb.Set(rdb.Context(), getStateKey(chatID), "creating_question_"+testName, 0).Err()
		if err != nil {
			log.Printf("Ошибка сохранения состояния в Redis: %v", err)
			return
		}

		// Запрашиваем у пользователя текст вопроса
		msg := tgbotapi.NewMessage(chatID, "Введите текст вопроса:")
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения: %v", err)
		}
	}

	// Ответ на callback, чтобы убрать "часики" на кнопке
	callback := tgbotapi.NewCallback(callbackQuery.ID, "")
	if _, err := bot.Request(callback); err != nil {
		log.Printf("Ошибка отправки callback: %v", err)
	}
}

// Обработка ввода пользователя
func HandleUserInput(bot *tgbotapi.BotAPI, message *tgbotapi.Message, rdb *redis.Client) {
	chatID := message.Chat.ID

	// Получаем состояние пользователя из Redis
	state, err := rdb.Get(rdb.Context(), getStateKey(chatID)).Result()
	if err == redis.Nil {
		// Состояние не установлено, игнорируем ввод
		return
	} else if err != nil {
		log.Printf("Ошибка получения состояния из Redis: %v", err)
		return
	}

	switch {
	case state == "creating_discipline":
		// Получаем название дисциплины из сообщения пользователя
		disciplineName := message.Text

		// Проверяем, существует ли уже такая дисциплина
		disciplines, err := getDisciplines(rdb, chatID)
		if err != nil {
			log.Printf("Ошибка получения списка дисциплин: %v", err)
			msg := tgbotapi.NewMessage(chatID, "Ошибка при проверке списка дисциплин.")
			bot.Send(msg)
			return
		}

		// Проверяем, есть ли уже такая дисциплина
		disciplineExists := false
		for _, discipline := range disciplines {
			if discipline == disciplineName {
				disciplineExists = true
				break
			}
		}

		if disciplineExists {
			// Если дисциплина уже существует, отправляем сообщение
			msg := tgbotapi.NewMessage(chatID, "Дисциплина уже создана. Перейдите в нее для просмотра тестов.")
			bot.Send(msg)
			return
		}

		// Сбрасываем состояние пользователя
		err = rdb.Del(rdb.Context(), getStateKey(chatID)).Err()
		if err != nil {
			log.Printf("Ошибка сброса состояния в Redis: %v", err)
		}

		// Добавляем дисциплину в список пользователя
		err = addDiscipline(rdb, chatID, disciplineName)
		if err != nil {
			log.Printf("Ошибка добавления дисциплины в Redis: %v", err)
			msg := tgbotapi.NewMessage(chatID, "Ошибка при создании дисциплины.")
			bot.Send(msg)
			return
		}

		// Отправляем подтверждение с inline-кнопками
		msg := tgbotapi.NewMessage(chatID, "Вы создали дисциплину: "+disciplineName)
		msg.ReplyMarkup = tgbotapi.NewInlineKeyboardMarkup(
			tgbotapi.NewInlineKeyboardRow(
				tgbotapi.NewInlineKeyboardButtonData("Создать тест", "create_test_"+disciplineName),
				tgbotapi.NewInlineKeyboardButtonData("Список тестов", "list_tests_"+disciplineName),
			),
		)
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения: %v", err)
		}

	case strings.HasPrefix(state, "creating_test_"):
		// Извлекаем название дисциплины из состояния
		disciplineName := strings.TrimPrefix(state, "creating_test_")
		log.Printf("Создание теста для дисциплины: %s", disciplineName)

		// Получаем название теста из сообщения пользователя
		testName := message.Text
		log.Printf("Название теста: %s", testName)

		// Сбрасываем состояние пользователя
		err := rdb.Del(rdb.Context(), getStateKey(chatID)).Err()
		if err != nil {
			log.Printf("Ошибка сброса состояния в Redis: %v", err)
		}

		// Добавляем тест в список тестов дисциплины
		err = addTest(rdb, chatID, disciplineName, testName)
		if err != nil {
			log.Printf("Ошибка добавления теста в Redis: %v", err)
			msg := tgbotapi.NewMessage(chatID, "Ошибка при создании теста.")
			bot.Send(msg)
			return
		}

		// Логирование успешного создания теста
		log.Printf("Тест «%s» успешно создан для дисциплины «%s»", testName, disciplineName)

		// Отправляем подтверждение с inline-кнопками
		msg := tgbotapi.NewMessage(chatID, fmt.Sprintf("Тест под названием «%s» успешно создан! Можно приступать к созданию вопросов.", testName))
		msg.ReplyMarkup = tgbotapi.NewInlineKeyboardMarkup(
			tgbotapi.NewInlineKeyboardRow(
				tgbotapi.NewInlineKeyboardButtonData("Создать вопрос", "create_question_"+testName),
				tgbotapi.NewInlineKeyboardButtonData("Просмотр вопросов", "view_questions_"+testName),
			),
		)
		if _, err := bot.Send(msg); err != nil {
			log.Printf("Ошибка отправки сообщения с кнопками: %v", err)
		}

	case strings.HasPrefix(state, "creating_question_"):
		// Извлекаем название теста из состояния
		testName := strings.TrimPrefix(state, "creating_question_")
		log.Printf("Создание вопроса для теста: %s", testName)

		// Получаем текущий вопрос из Redis (если он уже начат)
		questionKey := getQuestionKey(chatID, testName)
		questionData, err := rdb.Get(rdb.Context(), questionKey).Result()
		if err == redis.Nil {
			// Если вопрос еще не начат, сохраняем текст вопроса
			log.Printf("Вопрос еще не начат. Сохраняем текст вопроса.")
			questionData = fmt.Sprintf(`{"question": "%s"}`, message.Text)
			err = rdb.Set(rdb.Context(), questionKey, questionData, 0).Err()
			if err != nil {
				log.Printf("Ошибка сохранения вопроса в Redis: %v", err)
				return
			}

			// Запрашиваем количество вариантов ответа
			msg := tgbotapi.NewMessage(chatID, "Вопрос успешно создан. Сколько вариантов ответа будет у этого вопроса?")
			if _, err := bot.Send(msg); err != nil {
				log.Printf("Ошибка отправки сообщения: %v", err)
			}

			// Устанавливаем состояние "waiting_for_answer_count"
			err = rdb.Set(rdb.Context(), getStateKey(chatID), "waiting_for_answer_count_"+testName, 0).Err()
			if err != nil {
				log.Printf("Ошибка сохранения состояния в Redis: %v", err)
			} else {
				log.Printf("Состояние пользователя изменено на waiting_for_answer_count_")
			}

		} else if err != nil {
			log.Printf("Ошибка получения данных вопроса из Redis: %v", err)
			return
		} else {
			// Если вопрос уже начат, проверяем текущее состояние
			if strings.HasPrefix(state, "waiting_for_answer_count_") {
				// Пользователь ввел количество вариантов ответа
				log.Printf("Пользователь ввел количество вариантов ответа: %s", message.Text)
				answerCount, err := strconv.Atoi(message.Text)
				if err != nil || answerCount <= 0 {
					msg := tgbotapi.NewMessage(chatID, "Пожалуйста, введите корректное число (больше 0).")
					if _, err := bot.Send(msg); err != nil {
						log.Printf("Ошибка отправки сообщения: %v", err)
					}
					return
				}

				// Сохраняем количество вариантов ответа
				var question map[string]interface{}
				if err := json.Unmarshal([]byte(questionData), &question); err != nil {
					log.Printf("Ошибка десериализации вопроса: %v", err)
					return
				}
				question["answer_count"] = answerCount
				question["current_answer"] = 1 // Начинаем с первого варианта

				// Сохраняем обновленный вопрос в Redis
				updatedQuestionData, err := json.Marshal(question)
				if err != nil {
					log.Printf("Ошибка сериализации вопроса: %v", err)
					return
				}
				err = rdb.Set(rdb.Context(), questionKey, string(updatedQuestionData), 0).Err()
				if err != nil {
					log.Printf("Ошибка сохранения вопроса в Redis: %v", err)
					return
				}

				// Запрашиваем первый вариант ответа
				msg := tgbotapi.NewMessage(chatID, "Введите ответ 1:")
				if _, err := bot.Send(msg); err != nil {
					log.Printf("Ошибка отправки сообщения: %v", err)
				}

				// Устанавливаем состояние "waiting_for_answer"
				err = rdb.Set(rdb.Context(), getStateKey(chatID), "waiting_for_answer_"+testName, 0).Err()
				if err != nil {
					log.Printf("Ошибка сохранения состояния в Redis: %v", err)
				} else {
					log.Printf("Состояние пользователя изменено на waiting_for_answer_")
				}

			} else if strings.HasPrefix(state, "waiting_for_answer_") {
				// Пользователь ввел вариант ответа
				log.Printf("Пользователь ввел вариант ответа: %s", message.Text)
				var question map[string]interface{}
				if err := json.Unmarshal([]byte(questionData), &question); err != nil {
					log.Printf("Ошибка десериализации вопроса: %v", err)
					return
				}

				// Сохраняем вариант ответа
				currentAnswer := int(question["current_answer"].(float64))
				answerKey := fmt.Sprintf("answer_%d", currentAnswer)
				question[answerKey] = message.Text

				// Увеличиваем счетчик текущего варианта ответа
				question["current_answer"] = currentAnswer + 1

				// Сохраняем обновленный вопрос в Redis
				updatedQuestionData, err := json.Marshal(question)
				if err != nil {
					log.Printf("Ошибка сериализации вопроса: %v", err)
					return
				}
				err = rdb.Set(rdb.Context(), questionKey, string(updatedQuestionData), 0).Err()
				if err != nil {
					log.Printf("Ошибка сохранения вопроса в Redis: %v", err)
					return
				}

				// Проверяем, все ли варианты ответа введены
				answerCount := int(question["answer_count"].(float64))
				if currentAnswer >= answerCount {
					// Все варианты ответа введены, завершаем создание вопроса
					log.Printf("Все варианты ответа введены. Завершаем создание вопроса.")

					// Сохраняем вопрос в Redis
					questionsKey := fmt.Sprintf("user:%d:%s:questions", chatID, testName)
					err = rdb.RPush(rdb.Context(), questionsKey, string(updatedQuestionData)).Err()
					if err != nil {
						log.Printf("Ошибка сохранения вопроса в Redis: %v", err)
					}

					// Удаляем временный ключ вопроса
					err = rdb.Del(rdb.Context(), questionKey).Err()
					if err != nil {
						log.Printf("Ошибка удаления вопроса из Redis: %v", err)
					}

					// Сбрасываем состояние пользователя
					err = rdb.Del(rdb.Context(), getStateKey(chatID)).Err()
					if err != nil {
						log.Printf("Ошибка сброса состояния в Redis: %v", err)
					}

					// Отправляем сообщение об успешном создании вопроса
					msg := tgbotapi.NewMessage(chatID, "Вопрос успешно создан!")
					if _, err := bot.Send(msg); err != nil {
						log.Printf("Ошибка отправки сообщения: %v", err)
					}

					return
				}

				// Запрашиваем следующий вариант ответа
				msg := tgbotapi.NewMessage(chatID, fmt.Sprintf("Введите ответ %d:", currentAnswer+1))
				if _, err := bot.Send(msg); err != nil {
					log.Printf("Ошибка отправки сообщения: %v", err)
				}
			}
		}
	}
}

// Добавление дисциплины в список пользователя
func addDiscipline(rdb *redis.Client, chatID int64, disciplineName string) error {
	// Получаем текущий список дисциплин
	disciplines, err := getDisciplines(rdb, chatID)
	if err != nil {
		return err
	}

	// Добавляем новую дисциплину
	disciplines = append(disciplines, disciplineName)

	// Сохраняем обновленный список в Redis
	disciplinesJSON, err := json.Marshal(disciplines)
	if err != nil {
		return fmt.Errorf("ошибка сериализации списка дисциплин: %v", err)
	}

	err = rdb.Set(rdb.Context(), getDisciplinesKey(chatID), disciplinesJSON, 0).Err()
	if err != nil {
		return fmt.Errorf("ошибка сохранения списка дисциплин в Redis: %v", err)
	}

	return nil
}

// Добавление теста в список тестов дисциплины
func addTest(rdb *redis.Client, chatID int64, disciplineName, testName string) error {
	// Получаем текущий список тестов
	tests, err := getTests(rdb, chatID, disciplineName)
	if err != nil {
		return err
	}

	// Добавляем новый тест
	tests = append(tests, testName)

	// Сохраняем обновленный список в Redis
	testsJSON, err := json.Marshal(tests)
	if err != nil {
		return fmt.Errorf("ошибка сериализации списка тестов: %v", err)
	}

	err = rdb.Set(rdb.Context(), getTestsKey(chatID, disciplineName), testsJSON, 0).Err()
	if err != nil {
		return fmt.Errorf("ошибка сохранения списка тестов в Redis: %v", err)
	}

	return nil
}

// Получение списка дисциплин пользователя
func getDisciplines(rdb *redis.Client, chatID int64) ([]string, error) {
	// Получаем список дисциплин из Redis
	disciplinesJSON, err := rdb.Get(rdb.Context(), getDisciplinesKey(chatID)).Result()
	if err == redis.Nil {
		// Если список пуст, возвращаем пустой массив
		return []string{}, nil
	} else if err != nil {
		return nil, fmt.Errorf("ошибка получения списка дисциплин из Redis: %v", err)
	}

	// Десериализуем JSON в массив строк
	var disciplines []string
	if err := json.Unmarshal([]byte(disciplinesJSON), &disciplines); err != nil {
		return nil, fmt.Errorf("ошибка десериализации списка дисциплин: %v", err)
	}

	return disciplines, nil
}

// Получение списка тестов дисциплины
func getTests(rdb *redis.Client, chatID int64, disciplineName string) ([]string, error) {
	// Получаем список тестов из Redis
	testsJSON, err := rdb.Get(rdb.Context(), getTestsKey(chatID, disciplineName)).Result()
	if err == redis.Nil {
		// Если список пуст, возвращаем пустой массив
		return []string{}, nil
	} else if err != nil {
		return nil, fmt.Errorf("ошибка получения списка тестов из Redis: %v", err)
	}

	// Десериализуем JSON в массив строк
	var tests []string
	if err := json.Unmarshal([]byte(testsJSON), &tests); err != nil {
		return nil, fmt.Errorf("ошибка десериализации списка тестов: %v", err)
	}

	return tests, nil
}
