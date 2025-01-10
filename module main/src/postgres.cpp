
#include "postgres.h"


extern std::string PasswordPostgreSQL = "1234";
extern std::string dbName = "db_module";

std::string get_db_name()
{
    return dbName;
}

std::string get_db_password()
{
    return PasswordPostgreSQL;
}


void PostgresInit()
{
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
                        "disciplines INT[], "               // Список дисциплин
                        "banned BOOLEAN DEFAULT FALSE)"     // Бан
                    );
                } else if (table == "disciplines") {
                    txn.exec(
                        "CREATE TABLE disciplines ("
                        "id SERIAL PRIMARY KEY, "               // ID дисциплины
                        "name VARCHAR(100) NOT NULL UNIQUE, "   // Название дисциплины
                        "description TEXT, "                    // Описание
                        "teacher_id INT REFERENCES users(id))"  // ID преподавателя
                    );
                } else if (table == "tests") {
                    txn.exec(
                        "CREATE TABLE tests ("
                        "id SERIAL PRIMARY KEY, "                           // ID теста
                        "discipline_id INT REFERENCES disciplines(id), "    // ID дисциплины
                        "author_id INT REFERENCES users(id), "              // ID автора
                        "name TEXT NOT NULL, "                              // Название теста
                        "question_ids INT[], "                              // Список ID вопросов (массив)
                        "attempt_ids INT[], "                               // Список ID попыток (массив)
                        "is_active BOOLEAN DEFAULT TRUE, "                  // Активен ли тест
                        "deleted BOOLEAN DEFAULT FALSE)"                    // Флаг удаления теста
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
                        "correct_option_index INT, "            // Номер правильного ответа
                        "deleted BOOLEAN DEFAULT FALSE)"        // Флаг удаления вопроса
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







void add_user() {
    
    std::string last_name = "user_lastName2";
    std::string first_name = "user_firstName2";
    std::string middle_name = "user_middleName2";
    std::vector<std::string> roles;
    for (int i = 0; i < 5; i++)
        roles.push_back("role" + std::to_string(i));
    bool banned = false;
    std::string password = PasswordPostgreSQL;


    // Установка кодировки консоли на UTF-8
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");

    try {
        // Подключение к базе данных
        pqxx::connection conn("host=127.0.0.1 dbname=" + dbName + " user=postgres password=" + password);

        // Создание транзакции
        pqxx::work txn(conn);

        // Подготовка массива ролей для SQL-запроса
        std::string roles_array = "{";
        for (size_t i = 0; i < roles.size(); ++i) {
            roles_array += "\"" + txn.esc(roles[i]) + "\""; // Экранируем и заключаем в одинарные кавычки
            if (i < roles.size() - 1) {
                roles_array += ", ";
            }
        }
        roles_array += "}";
        // SQL-запрос для добавления пользователя
        std::string query = "INSERT INTO users (last_name, first_name, middle_name, roles, banned) "
                            "VALUES ('" + txn.esc(last_name) + "', '" + txn.esc(first_name) + "', '" + txn.esc(middle_name) + "', "
                            "'" + roles_array + "', " + (banned ? "TRUE" : "FALSE") + ")";

        // Выполнение запроса
        txn.exec(query);

        // Завершение транзакции
        txn.commit();

        std::cout << "Пользователь успешно добавлен." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Ошибка при добавлении пользователя: " << e.what() << std::endl;
    }
}





std::vector<int> sql_get_array_int (const std::string& column_name, const std::string& table_name, int id)
{
    std::vector<int> arr;

    try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);       //  начало транзакции
            std::string query = "SELECT " + column_name + " FROM " + table_name + " WHERE id = " + txn.quote(id);
            pqxx::result result = txn.exec(query);

            if (!result.empty()) {
                pqxx::array_parser parser = result[0][0].as_array();
                std::pair<pqxx::array_parser::juncture, std::string> elem;

                while ((elem = parser.get_next()).first != pqxx::array_parser::juncture::done) {
                    if (elem.first == pqxx::array_parser::juncture::string_value) {
                        arr.push_back(std::stoi(elem.second));
                    }
                }
            } else {
                std::cout << "   Error." << std::endl;
            }

            txn.commit();               //  конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }

    return arr;
}




std::vector<std::string> sql_get_array_str(const std::string& column_name, const std::string& table_name, int id)
{
    std::vector<std::string> arr;

    try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);       //  начало транзакции
            std::string query = "SELECT " + column_name + " FROM " + table_name + " WHERE id = " + txn.quote(id);
            pqxx::result result = txn.exec(query);

            if (!result.empty()) {
                pqxx::array_parser parser = result[0][0].as_array();
                std::pair<pqxx::array_parser::juncture, std::string> elem;

                while ((elem = parser.get_next()).first != pqxx::array_parser::juncture::done) {
                    if (elem.first == pqxx::array_parser::juncture::string_value) {
                        arr.push_back(elem.second);
                    }
                }
            } else {
                std::cout << "   Error." << std::endl;
            }

            txn.commit();               //  конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }

    return arr;
}






std::vector<std::string> sql_get_list_str (const std::string& column_name, const std::string& table_name)
{
    std::vector<std::string> list;

    try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);       //  начало транзакции
            pqxx::result result = txn.exec("SELECT " + column_name + " FROM " + table_name);

            for (auto row : result) {
                list.push_back(row[0].as<std::string>());
            }
            txn.commit();               //  конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }

    return list;
}



std::vector<int> sql_get_list_int (const std::string& column_name, const std::string& table_name)
{
    std::vector<int> list;

    try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);       //  начало транзакции
            pqxx::result result = txn.exec("SELECT " + column_name + " FROM " + table_name);

            for (auto row : result) {
                list.push_back(row[0].as<int>());
            }
            txn.commit();               //  конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }

    return list;
}


std::string sql_get_one_str(const std::string& column_name, const std::string& table_name, int id)
{
    std::string one = "";

    try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);       //  начало транзакции
            pqxx::result result = txn.exec("SELECT " + column_name + " FROM " + table_name + " WHERE id = " + std::to_string(id));

            auto row = result[0];
            one = row[0].as<std::string>();
            
            txn.commit();               //  конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }

    return one;
}

int sql_get_one_int(const std::string& column_name, const std::string& table_name, int id)
{
    int one = -3;

    try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);       //  начало транзакции
            pqxx::result result = txn.exec("SELECT " + column_name + " FROM " + table_name + " WHERE id = " + std::to_string(id));

            auto row = result[0];
            one = row[0].as<int>();
            
            txn.commit();               //  конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }

    return one;
}

bool sql_get_one_bool(const std::string& column_name, const std::string& table_name, int id)
{
    bool one = false;

    try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);       //  начало транзакции
            pqxx::result result = txn.exec("SELECT " + column_name + " FROM " + table_name + " WHERE id = " + std::to_string(id));

            auto row = result[0];
            one = row[0].as<bool>();
            
            txn.commit();               //  конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }

    return one;
}


void sql_update_one_str(const std::string& column_name, const std::string& table_name, int id, const std::string new_value)
{
    try {
        // Подключение к базе данных
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);  // Начало транзакции
            std::string query = 
                "UPDATE " + table_name + " SET " + column_name + " = " + txn.quote(new_value) + " WHERE id = " + txn.quote(id);
            txn.exec(query);
            txn.commit();           // конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }
}


void sql_update_one_bool(const std::string& column_name, const std::string& table_name, int id, bool new_value)
{
    try {
        // Подключение к базе данных
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);  // Начало транзакции
            std::string query = 
                "UPDATE " + table_name + " SET " + column_name + " = " + txn.quote(new_value) + " WHERE id = " + txn.quote(id);
            txn.exec(query);
            txn.commit();           // конец транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }
}




void sql_del_from_array_int(const std::string& column_name, const std::string& table_name, int id, int num) {
    try {
        // Подключение к базе данных
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn); // Начало транзакции

            // Формируем SQL-запрос для удаления элемента из массива
            std::string query = "UPDATE " + table_name + " SET " + column_name + 
            " = array_remove(" + column_name + ", " + txn.quote(num) + ") WHERE id = " + txn.quote(id);
            
            // Выполняем запрос
            txn.exec(query);

            txn.commit(); // Завершение транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }
}



void sql_add_to_array_int(const std::string& column_name, const std::string& table_name, int id, int num)
{
    try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");
        pqxx::work txn(conn);

        txn.exec_params(
            "UPDATE " + table_name +
            " SET " + column_name + " = array_append(" + column_name + ", $1) "
            "WHERE id = $2",
            num, id
        );
        txn.commit();
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }
}