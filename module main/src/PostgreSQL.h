#include <pqxx/pqxx>
#include <iostream>
#include <windows.h> // Для SetConsoleOutputCP

void PostgresInit()
{
    std::string PasswordPostgreSQL = "1234"; // Пароль для подключения к PostgreSQL
    std::string dbName = "db_module"; // Имя базы данных, которую нужно проверить или создать 

    // Установка кодировки консоли на UTF-8
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");

    while (true)
    {
        std::cout << "Введите пароль для PostgreSQL: ";
        std::cin >> PasswordPostgreSQL;

        // Формирование строки подключения
        std::string connInfo = "host=127.0.0.1 dbname=postgres user=postgres password=" + PasswordPostgreSQL;

        try
        {
            // Попытка подключения к базе данных
            pqxx::connection conn(connInfo);

            if (conn.is_open())
            {
                std::cout << "Подключение к базе данных успешно установлено!" << std::endl;
                conn.close();
                break; // Выход из цикла, если подключение успешно
            }
            else
            {
                std::cerr << "Не удалось подключиться к базе данных." << std::endl;
            }
        }
        catch (const pqxx::sql_error &e)
        {
            std::cerr << "Ошибка SQL: " << e.what() << std::endl;
            std::cerr << "Запрос: " << e.query() << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }
    

    try {
        // Подключение к серверу PostgreSQL (без указания конкретной базы данных)
        pqxx::connection conn("host=127.0.0.1 user=postgres password=" + PasswordPostgreSQL);

        // Создание транзакции для проверки существования базы данных
        pqxx::work txn(conn);

        // Выполнение запроса для проверки существования базы данных
        pqxx::result result = txn.exec("SELECT 1 FROM pg_database WHERE datname = '" + dbName + "'");

        // Проверка результата
        if (!result.empty()) {
            std::cout << "База данных 'db_module' существует." << std::endl;
        } else {
            std::cout << "База данных 'db_module' не существует. Создание..." << std::endl;

            // Завершаем текущую транзакцию, так как CREATE DATABASE не может выполняться внутри транзакции
            txn.commit();

            // Создаем новое соединение для выполнения CREATE DATABASE
            pqxx::connection conn_create("host=127.0.0.1 user=postgres password=" + PasswordPostgreSQL);
            pqxx::nontransaction nontxn(conn_create);

            // Выполнение команды создания базы данных
            nontxn.exec("CREATE DATABASE " + dbName);
            std::cout << "База данных 'db_module' успешно создана." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    try {
        // Подключение к базе данных dbName
        pqxx::connection conn("host=127.0.0.1 dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL);

        // Создание транзакции для проверки существования таблиц
        pqxx::work txn(conn);

        // Список таблиц, которые нужно проверить и создать
        std::string tables[] = {"users", "disciplines", "tests", "questions", "attempts", "answers"};

        // Проверка существования каждой таблицы
        for (const auto& table : tables) {
            pqxx::result result = txn.exec(
                "SELECT 1 FROM information_schema.tables WHERE table_schema = 'public' AND table_name = '" + table + "'"
            );

            if (!result.empty()) {
                std::cout << "Таблица '" << table << "' существует." << std::endl;
            } else {
                std::cout << "Таблица '" << table << "' не существует. Создание..." << std::endl;

                // Создание таблицы в зависимости от её имени
                if (table == "users") {
                    txn.exec(
                        "CREATE TABLE users ("
                        "id SERIAL PRIMARY KEY, "           // ID пользователя
                        "last_name VARCHAR(50) NOT NULL, "  // Фамилия
                        "first_name VARCHAR(50) NOT NULL, " // Имя
                        "middle_name VARCHAR(50), "         // Отчество
                        "roles TEXT[], "                    // Список ролей (массив)
                        "banned BOOLEAN DEFAULT FALSE)"     // Бан
                    );
                } else if (table == "disciplines") {
                    txn.exec(
                        "CREATE TABLE disciplines ("
                        "id SERIAL PRIMARY KEY, "               // ID дисциплины
                        "name VARCHAR(100) NOT NULL UNIQUE, "   // Название дисциплины
                        "description TEXT, "                    // Описание
                        "teacher_id INT REFERENCES users(id), " // ID преподавателя
                        "student_ids INT[])"                    // Список ID студентов (массив)
                    );
                } else if (table == "tests") {
                    txn.exec(
                        "CREATE TABLE tests ("
                        "id SERIAL PRIMARY KEY, "                           // ID теста
                        "discipline_id INT REFERENCES disciplines(id), "    // ID дисциплины
                        "author_id INT REFERENCES users(id), "              // ID автора
                        "question_ids INT[], "                              // Список ID вопросов (массив)
                        "attempt_ids INT[], "                               // Список ID попыток (массив)
                        "is_active BOOLEAN DEFAULT TRUE)"                   // Активен ли тест
                    );
                } else if (table == "questions") {
                    txn.exec(
                        "CREATE TABLE questions ("
                        "id SERIAL PRIMARY KEY, "               // ID вопрос
                        "author_id INT REFERENCES users(id), "  // ID автора
                        "version INT NOT NULL, "                // Версия вопроса
                        "title VARCHAR(255) NOT NULL, "         // Название вопроса
                        "description TEXT, "                    // Описание вопроса
                        "options TEXT[], "                      // Список вариантов ответов (массив)
                        "correct_option_index INT)"             // Номер правильного ответа
                    );
                } else if (table == "attempts") {
                    txn.exec(
                        "CREATE TABLE attempts ("
                        "id SERIAL PRIMARY KEY, "                           // ID попытки
                        "user_id INT REFERENCES users(id), "                // ID пользователя
                        "test_id INT REFERENCES tests(id), "                // ID теста
                        "discipline_id INT REFERENCES disciplines(id), "    // ID дисциплины
                        "answer_ids INT[], "                                // Список ID ответов (массив)
                        "is_completed BOOLEAN DEFAULT FALSE)"               // Завершена ли попытка
                    );
                } else if (table == "answers") {
                    txn.exec(
                        "CREATE TABLE answers ("
                        "id SERIAL PRIMARY KEY, "                       // ID ответа
                        "attempt_id INT REFERENCES attempts(id), "      // ID попытки
                        "question_id INT REFERENCES questions(id), "    // ID вопроса
                        "answer_index INT)"                             // Индекс ответа
                    );
                }

                std::cout << "Таблица '" << table << "' успешно создана." << std::endl;
            }
        }

        // Завершение транзакции
        txn.commit();
    } catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    std::cout << std::endl;
}

