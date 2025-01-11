
#include "handlerRequest.h"


bool Unauthorized(httplib::Response& res, std::unordered_map<jwt::traits::kazuho_picojson::string_type, jwt::claim> permission)
{
    return false;
    if (permission.empty()) {
        std::cout << "   Token not found or invalid. return status 401" << std::endl;
        res.status = 401; // Unauthorized
        res.set_content("Unauthorized: Token not found or invalid.", "text/plain");
        return true;
    }
    std::cout << "   processing..." << std::endl;
    return false;
}





void AddUser(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   добавляет в БД Users ФИО и id нового пользователя
    *   только для модуля авторизации
    */

    std::cout << "Add new user. Student + Teacher." << std::endl;

    int uid = nlohmann::json::parse(req.body)["uid"];
    std::string last_name = "user_lastName-" + std::to_string(uid);
    std::string first_name = "user_firstName-" + std::to_string(uid);
    std::string middle_name = "user_middleName-" + std::to_string(uid);
    std::vector<std::string> roles;
    roles.push_back("Student");
    roles.push_back("Teacher");
    bool banned = false;

    // Установка кодировки консоли на UTF-8
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");

    try {
        pqxx::connection conn("host=127.0.0.1 dbname=" + get_db_name() + " user=postgres password=" + get_db_password());
        pqxx::work txn(conn);

        // Подготовка массива ролей для SQL-запроса
        std::string roles_array = "{";
        for (size_t i = 0; i < roles.size(); ++i) {
            roles_array += "\"" + txn.esc(roles[i]) + "\"";
            if (i < roles.size() - 1) {
                roles_array += ", ";
            }
        }
        roles_array += "}";
        // SQL-запрос для добавления пользователя
        std::string query = "INSERT INTO users (id, last_name, first_name, middle_name, roles, banned) "
                            "VALUES ('"+ txn.esc(std::to_string(uid)) + "', '" + txn.esc(last_name) + "', '" + txn.esc(first_name) + "', '" + txn.esc(middle_name) + "', "
                            "'" + roles_array + "', " + (banned ? "TRUE" : "FALSE") + ")";
        txn.exec(query);
        txn.commit();       // Завершение транзакции

        std::cout << "User added successfully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error adding user: " << e.what() << std::endl;
    }
}






//          ПОЛЬЗОВАТЕЛИ


//  Посмотреть список пользователей
void GetUserList(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает массив содержащий ФИО и ID каждого пользователя зарегистрированного в системе
    *   -
    */

    std::cout << "Get user-list:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::vector<int> uid           = sql_get_list_int("id", "users");
    std::vector<std::string> lname = sql_get_list_str("last_name", "users");
    std::vector<std::string> fname = sql_get_list_str("first_name", "users");
    std::vector<std::string> mname = sql_get_list_str("middle_name", "users");

    nlohmann::json jsonRes;
    jsonRes["users"] = nlohmann::json::array();
    for (int i = 0; i < uid.size(); i++)
    {
        nlohmann::json user;
        user["id"] = uid[i];
        user["last_name"] = lname[i];
        user["first_name"] = fname[i];
        user["middle_name"] = mname[i];
        jsonRes["users"].push_back(user);
    }
    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о пользователе (ФИО)
void GetUserNamea(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает ФИО пользователя по его ID
    *   + О себе
    *   + О другом
    */

    std::cout << "Get user name:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int id = std::stoi(req.matches[1]);
    nlohmann::json jsonRes;
    jsonRes["last_name"]   = sql_get_one_str("last_name", "users", id);
    jsonRes["first_name"]  = sql_get_one_str("first_name", "users", id);
    jsonRes["middle_name"] = sql_get_one_str("middle_name", "users", id);
    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Изменить ФИО пользователя
void SetUserName(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Заменяет ФИО пользователя на указанное по его ID
    *   + Себе
    *   - Другому
    */

    std::cout << "Set user name:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    
    nlohmann::json body = nlohmann::json::parse(req.body);
    int uid = std::stoi(req.matches[1]);
    std::string first_name = body["first_name"];    //  имя
    std::string last_name = body["last_name"];      //  фамилия
    std::string middle_name = body["middle_name"];  //  отчество

    sql_update_one_str("first_name", "users", uid, first_name);
    sql_update_one_str("last_name", "users", uid, last_name);
    sql_update_one_str("middle_name", "users", uid, middle_name);


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о пользователе (курсы)
void GetUserCourses(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает список дисциплин пользователя по его ID.
    *   + О себе
    *   - О другом
    */

    std::cout << "Get user courses:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    nlohmann::json jsonRes;
    jsonRes["disciplines"] = nlohmann::json::array();
    int uid = std::stoi(req.matches[1]);

    std::vector<int> disid = sql_get_array_int("disciplines", "users", uid);
    
    for (auto i : disid)
    {
        nlohmann::json discipline;
        discipline["id"] = i;
        discipline["name"] = sql_get_one_str("name", "disciplines", i);
        jsonRes["disciplines"].push_back(discipline);
    }

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о пользователе (оценки)
void GetUserGrades(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает список оценок пользователя по его ID.
    *   + О себе
    *   - О другом
    */

    std::cout << "Get user grades:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
        jsonRes.push_back("5");

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о пользователе (тесты)
void GetUserTests(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает список тестов пользователя по его ID.
    *   + О себе
    *   - О другом
    */

    std::cout << "Get user tests:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
        jsonRes.push_back(std::to_string(i));

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о пользователе (роли)
void GetUserRoles(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает массив ролей пользователя по его ID
    *   - Свои
    *   - Чужие
    */
    
    std::cout << "Get user roles:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int uid = std::stoi(req.matches[1]);
    nlohmann::json jsonRes;
    std::vector<std::string> roles = sql_get_array_str("roles", "users", uid);
    jsonRes["roles"] = nlohmann::json::array();
    for (auto i : roles)
        jsonRes["roles"].push_back(i);
    
    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Изменить роли пользователя
void SetUserRoles(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Заменяет роли пользователя на указанные по его ID
    *   - Себе
    *   - Другому
    */

    std::cout << "Set user roles:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO


    std::cout << "   End." << std::endl;
}

//  Посмотреть заблокирован ли пользователь
void GetUserBlock(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для пользователя с указанным ID возвращает значение показывающее заблокирован пользователь или нет
    *   - О себе
    *   - О другом
    */
    
    std::cout << "Get user block:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int uid = std::stoi(req.matches[1]);
    nlohmann::json jsonRes;
    jsonRes["banned"] = sql_get_one_bool("banned", "users", uid);
    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  заблокировать пользователя
void SetUserBlock(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для пользователя запрещены все действия, даже те, которые разрешены по умолчанию. На любой запрос нужно отвечать кодом 418
    *   - Себя
    *   - Другого
    */
    
    std::cout << "Set user block:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int uid = std::stoi(req.matches[1]);
    sql_update_one_bool("banned", "users", uid, true);
    std::cout << "   User " << std::to_string(uid) << " banned." << std::endl; 


    std::cout << "   End." << std::endl;
}

//  разблокировать пользователя
void SetUserUnblock(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для пользователя запрещены все действия, даже те, которые разрешены по умолчанию. На любой запрос нужно отвечать кодом 418
    *   - Себя
    *   - Другого
    */

    std::cout << "Set user unblock:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int uid = std::stoi(req.matches[1]);
    sql_update_one_bool("banned", "users", uid, false);
    std::cout << "   User " << std::to_string(uid) << " unbanned." << std::endl; 


    std::cout << "   End." << std::endl;
}






//          ДИСЦИПЛИНЫ


//   Возвращает массив содержащий Название, Описание и ID каждой дисциплины зарегистрированной в системе
void GetDisceplines(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает массив содержащий Название, Описание и ID каждой дисциплины зарегистрированной в системе
    *   +
    */

    std::cout << "Get disciplines:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    nlohmann::json jsonRes;
    jsonRes["disciplines"] = nlohmann::json::array();
    std::vector<int> disid = sql_get_list_int("id", "disciplines");
    std::vector<std::string> disciplines = sql_get_list_str("name", "disciplines");
    for (int i = 0; i < disid.size(); i++)
    {
        nlohmann::json discipline;
        discipline["name"] = disciplines[i];
        discipline["id"] = disid[i];
        jsonRes["disciplines"].push_back(discipline);
    }

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Возвращает Название, Описание, ID преподавателя для дисциплины по её ID
void GetDisceplineInfo(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает Название, Описание, ID преподавателя для дисциплины по её ID
    *   +
    */

    std::cout << "Get discipline info:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id = req.matches[1];
    std::cout << "   id: " << id << std::endl;

    int disid = std::stoi(req.matches[1]);
    nlohmann::json jsonRes;
    jsonRes["name"] = sql_get_one_str("name", "disciplines", disid);
    jsonRes["description"] = sql_get_one_str("description", "disciplines", disid);
    jsonRes["teacher_id"] = sql_get_one_int("teacher_id", "disciplines", disid);

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Изменить информацию о дисциплине (Название, Описание)
void SetDisceplineInfo(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Заменяет Название и(или) Описание дисциплины на указанные по её ID
    *   + Для своей дисциплины
    *   - Для чужих
    */

    std::cout << "Set discipline info:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];
    std::string description = body["description"];
    sql_update_one_str("name", "disciplines", disid, name);
    sql_update_one_str("description", "disciplines", disid, description);
    std::cout << "   New name: " << name << " New description: " << description << std::endl;


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о дисциплине (Список тестов)
void GetDisceplineTestList(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает массив содержащий Название и ID для каждого теста присутствующего в дисциплине по её ID
    *   + Для своей дисциплины
    *   + Для чужих, но если записан на неё
    *   - Для чужих
    */

    std::cout << "Get discepline tests:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    nlohmann::json jsonRes;
    jsonRes["tests"] = nlohmann::json::array();
    std::vector<int> tests = sql_get_list_int("id", "tests");
    for (auto i : tests)
    {
        if (sql_get_one_int("discipline_id", "tests", i) == disid)
        {
            nlohmann::json test;
            test["name"] = sql_get_one_str("name", "tests", disid);
            test["id"] = i;
            jsonRes["tests"].push_back(test);
        }
    }

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о тесте (Активный тест или нет)
void GetDisceplineTestActive(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для дисциплины с указанным ID и теста с указанным ID возвращает значение показывающее активен он или нет.
    *   Если тест НЕ активен, он отображается в списке, но пройти его нельзя
    *   + Для своей дисциплины
    *   + Для чужих, но если записан на неё
    *   - Для чужих
    */

    std::cout << "Get discepline test active:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int discid = std::stoi(req.matches[1]);
    int testid = std::stoi(req.matches[2]);
    std::cout << "   Disc: " << discid << " Test: " << testid << std::endl;

    nlohmann::json jsonRes;
    jsonRes["active"] = sql_get_one_bool("is_active", "tests", testid);

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Активировать тест
void SetDisceplineTestActivate(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для дисциплины с указанным ID и теста с указанным ID устанавливает значение активности.
    *   Если тест установлен в состояние Не активный, все начатые попытки автоматически отмечаются завершёнными
    *   + Для своей дисциплины
    *   - Для чужих
    */
    
    std::cout << "Activate discepline test:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    int testid = std::stoi(req.matches[2]);
    sql_update_one_bool("is_active", "tests", testid, true);
    std::cout << "   Active Disc: " << disid << " Test: " << testid << std::endl;


    std::cout << "   End." << std::endl;
}

//  Деактивировать тест
void SetDisceplineTestDeactivate(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для дисциплины с указанным ID и теста с указанным ID устанавливает значение активности.
    *   Если тест установлен в состояние Не активный, все начатые попытки автоматически отмечаются завершёнными
    *   + Для своей дисциплины
    *   - Для чужих
    */
    
    std::cout << "Deactivate discepline test:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    int testid = std::stoi(req.matches[2]);
    sql_update_one_bool("is_active", "tests", testid, false);
    std::cout << "   Deactive Disc: " << disid << " Test: " << testid << std::endl;


    std::cout << "   End." << std::endl;
}

//  Добавить тест в дисциплину по её id
void AddDisceplineTest(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Добавляет новый тест в дисциплину с ID новый тест с указанным названием, пустым списком вопросов и автором и возвращает ID теста.
    *   По умолчанию тест не активен.
    *   + Для своей дисциплины
    *   - Для чужих
    */

    std::cout << "Add discepline test:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO
    //  вытаскивать uid из permission

    int disid = std::stoi(req.matches[1]);
    int uid = (nlohmann::json::parse(req.body))["id"];
    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];

    try {
        // Подключаемся к базе данных
        pqxx::connection conn("dbname=" + get_db_name() + " user=postgres password=" + get_db_password() + " host=127.0.0.1 port=5432");
        pqxx::work txn(conn);

        // Выполняем SQL-запрос для вставки нового теста
        pqxx::result result = txn.exec_params(
            "INSERT INTO tests (discipline_id, author_id, name, question_ids, attempt_ids, is_active, deleted) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7) RETURNING id",
            disid, uid, name, "{}", "{}", false, false
        );

        // Получаем ID вставленного теста
        int test_id = result[0][0].as<int>();

        // Фиксируем транзакцию
        txn.commit();

        // Формируем JSON-ответ
        nlohmann::json jsonRes;
        jsonRes["id"] = test_id;

        std::cout << "   Add new test \"" << name << "\" id: " << test_id << std::endl;
        res.set_content(jsonRes.dump(), "application/json");
    } catch (const std::exception &e) {
        // Обработка ошибок
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content("Internal Server Error", "text/plain");
    }
    


    std::cout << "   End." << std::endl;
}

//  Удалить тест из дисциплины (id дисциплины и теста)
void DelDisceplineTest(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Отмечает тест как удалённый (реально ничего не удаляется). Все оценки перестают отображаться, но тоже не удаляются.
    *   + Для своей дисциплины
    *   - Для чужих
    */
    
    std::cout << "Del discepline test:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    int testid = std::stoi(req.matches[2]);
    sql_update_one_bool("deleted", "tests", testid, true);
    std::cout << "   Disc: " << disid << " Test: " << testid << std::endl;


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о дисциплине (Список студентов)
void GetDisceplineUserList(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает массив содержащий ID каждого студента записанного на дисциплину по её ID
    *   + Для своей дисциплины
    *   - Для чужих
    */
    
    std::cout << "Get discipline user-list:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    nlohmann::json jsonRes;
    jsonRes["users"] = nlohmann::json::array();
    std::vector<int> users = sql_get_list_int("id", "users");
    for (auto i : users)
    {
        std::vector<int> disciplines = sql_get_array_int("disciplines", "users", i);
        for (auto j : disciplines)
            if (j == disid)
                jsonRes["users"].push_back(i);
    }

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}  

//  Записать пользователя на дисциплину
void AddDisceplineUser(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Добавляет пользователя с указанным ID на дисциплину с указанным ID
    *   + Себя
    *   - Других
    */
    
    std::cout << "Add discipline user:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    int uid = std::stoi(req.matches[2]);
    std::cout << "   Disc: " << disid << " User: " << uid << std::endl;

    std::vector<int> user = sql_get_array_int("disciplines", "users", uid);
    bool find_disc = false;
    for (auto i : user) if (i == disid) find_disc = true;
    if (!find_disc) sql_add_to_array_int("disciplines", "users", uid, disid);


    std::cout << "   End." << std::endl;
}

//  Отчислить пользователя с дисциплины
void DelDisceplineUser(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Отчисляет пользователя с указанным ID с дисциплины с указанным ID
    *   + Себя
    *   - Других
    */
    
    std::cout << "Del discipline user:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    int uid = std::stoi(req.matches[2]);
    sql_del_from_array_int("disciplines", "users", uid, disid);


    std::cout << "   End." << std::endl;
}

//  Создать дисциплину
void AddDiscepline(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Создаёт дисциплину с указанным названием, описанием и преподавателем. Как результат возвращает её ID
    *   -
    */
    
    std::cout << "Add discepline:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];
    std::string description = body["description"];
    int idPrepod = std::stoi(std::string(body["id"]));

    try {
        pqxx::connection conn("dbname=" + get_db_name() + " user=postgres password=" + get_db_password() + " host=127.0.0.1 port=5432");

        if (conn.is_open()) {
            pqxx::work txn(conn);  // Начало транзакции
            std::string query = 
                "INSERT INTO disciplines (name, description, teacher_id) "
                "VALUES (" + 
                txn.quote(name) + ", " + 
                txn.quote(description) + ", " + 
                txn.quote(idPrepod) + ") RETURNING id";
            pqxx::result result = txn.exec(query);
            txn.commit();  // Завершение транзакции
        } else {
            std::cerr << "   Failed to connect to database." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }


    std::cout << "   End." << std::endl;
}

//  Удалить дисциплину
void DelDiscepline(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Отмечает дисциплину как удалённую (реально ничего не удаляется). Все тесты и оценки перестают отображаться, но тоже не удаляются.
    *   + Для своей дисциплины
    *   - Для чужих
    */
    
    std::cout << "Del discepline:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    std::string id = req.matches[1];
    std::cout << "   id: " << id << std::endl;


    std::cout << "   End." << std::endl;
}






//      ВОПРОСЫ


//  Посмотреть список вопросов
void GetQuestList(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает массив содержащий Название вопроса, его версию и ID автора для каждого теста в системе.
    *   Если у вопроса есть несколько версий, показывается только последняя
    *   + Свои
    *   - Чужие
    */
    
    std::cout << "Get quest list:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    nlohmann::json jsonRes;
    jsonRes["questoins"] = nlohmann::json::array();
    std::vector<std::string> questions = sql_get_list_str("title", "questions");
    std::vector<std::string> author = sql_get_list_str("author_id", "questions");
    std::vector<int> qid = sql_get_list_int("id", "questions");
    for (int i = 0; i < qid.size(); i++)
    {
        nlohmann::json quest;
        quest["title"] = questions[i];
        quest["id"] = qid[i];
        quest["author"] = author[i];
        jsonRes["questoins"].push_back(quest);
    }

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Посмотреть информацию о вопросе id вопроса и id версии вопроса
void GetQuestInfo(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для указанного ID вопроса и версии возвращает Название, Текст вопроса, Варианты ответов, Номер правильного ответа
    *   + Свои
    *   + Студент у которого есть попытка ответа содержащая этот вопрос
    *   - Остальные
    */
    
    std::cout << "Get quest info by id:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    std::string idQuest = req.matches[1];
    std::string version = req.matches[2];
    std::cout << "   idQuest: " << idQuest << " version: " << version << std::endl;

    nlohmann::json jsonRes;
    jsonRes["name"] = "name_quest";
    jsonRes["description"] = "description";
    jsonRes["options"] = nlohmann::json::array();
    for (int i = 0; i < 5; i++)
        jsonRes["options"].push_back("opt" + std::to_string(i));
    jsonRes["correct_answer"] = 1;

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Изменить текст вопроса/ответов (создаётся новая версия)
void SetQuestInfo(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для указанного ID вопроса создаёт новую версию с заданным Названием, Тексом вопроса, Вариантами ответов, Номером правильного ответа
    *   + Свои
    *   - Чужие
    */
    
    std::cout << "Set quest info:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int id = std::stoi(req.matches[1]);
    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];
    std::string description = body["description"];
    std::vector<std::string> options;
    for (const auto i : body["options"])
        options.push_back(i);
    int correct_answer = body["correct_answer"];


    std::cout << "   End." << std::endl;
}

//  Создать вопрос
void AddQuest(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Создаёт новый вопрос с заданным Названием, Тексом вопроса, Вариантами ответов, Номером правильного ответа.
    *   Версия вопроса 1. В качестве ответа возвращается ID вопроса
    *   -
    */
    
    std::cout << "Add quest:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];
    std::string description = body["description"];
    std::vector<std::string> options;
    for (const auto i : body["options"])
        options.push_back(i);
    int correct_answer = body["correct_answer"];
    int uid = body["id"];
    int version = 1;

    try {
        pqxx::connection conn("dbname=" + get_db_name() + " user=postgres password=" + get_db_password() + " host=127.0.0.1 port=5432");
        pqxx::work txn(conn);

        std::string options_array = "{";
        for (size_t i = 0; i < options.size(); ++i) {
            options_array += "\"" + options[i] + "\"";
            if (i < options.size() - 1) {
                options_array += ",";
            }
        }
        options_array += "}";

        // Вставка данных в таблицу вопросов
        std::string options_str = nlohmann::json(options).dump(); // Сериализация вариантов ответов в JSON строку
        std::string query = "INSERT INTO questions (author_id, version, title, description, options, correct_option_index) "
                          "VALUES (" + std::to_string(uid) + ", " + std::to_string(version) + ", " +
                          txn.quote(name) + ", " + txn.quote(description) + ", " +
                          txn.quote(options_array) + ", " + std::to_string(correct_answer) + ") RETURNING id;";

        pqxx::result result = txn.exec(query);
        txn.commit();

        // Получение ID вставленного вопроса
        int quest_id = result[0][0].as<int>();

        // Формирование ответа
        nlohmann::json response;
        response["id"] = quest_id;
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content("Internal Server Error", "text/plain");
    }


    std::cout << "   End." << std::endl;
}

//  Удалить вопрос
void DelQuest(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если вопрос не используется в тестах (даже удалённых), то вопрос отмечается как удалённый (но реально не удаляется)
    *   + Свой
    *   - Чужой
    */

    std::cout << "Del quest:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO


    std::cout << "   End." << std::endl;
}





//      ТЕСТЫ


//  Удалить вопрос из теста
void DelTestQuest(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если у теста ещё не было попыток прохождения, то удаляет у теста с указанным ID вопрос с указанным ID
    *   + Если пользователь преподаватель на курсе
    *   - Для остальных
    */
    
    std::cout << "Del quest from test:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    int idQuest = std::stoi(req.matches[3]);

    std::cout << "   Del test " << idTest << " quest " << idQuest << std::endl;


    std::cout << "   End." << std::endl;
}

//  Добавить вопрос в тест
void AddTestQuest(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если у теста ещё не было попыток прохождения, то добавляет в теста с указанным ID вопрос с указанным ID в последнюю позицию
    *   + Если пользователь преподаватель на курсе и автор вопроса
    *   - Для остальных
    */
    
    std::cout << "Add quest to test:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    int testid = std::stoi(req.matches[2]);
    int qid = std::stoi(req.matches[3]);

    std::vector<int> save_qid = sql_get_array_int("question_ids", "tests", testid);
    for (int i = 0; i < save_qid.size(); i++)
    {
        if (save_qid[i] == qid)
        {
            return;
        }
    }
    sql_add_to_array_int("question_ids", "tests", testid, qid);


    std::cout << "   End." << std::endl;
}

//  Изменить порядок следования вопросов в тесте
void SetTestQuestSequence(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если у теста ещё не было попыток прохождения, то для теста с указанным ID устанавливает указанную последовательность вопросов
    *   + Если пользователь преподаватель на курсе
    *   - Для остальных
    */
    
    std::cout << "Set quest sequence in test:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);


    std::cout << "   End." << std::endl;
}

//  Посмотреть список пользователей прошедших тест
void GetQuestUsers(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для теста с указанным ID выбирает все попытки и возвращает ID пользователей выполнивших эти попытки
    *   + Если пользователь преподаватель на курсе
    *   - Для остальных
    */
    
    std::cout << "Get quest users:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    nlohmann::json jsonRes;
    jsonRes["users"] = nlohmann::json::array();
    for (int i = 0; i < 5; i++)
        jsonRes["users"].push_back("user" + std::to_string(i));
    
    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Посмотреть оценку пользователя
void GetTestGreads(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для теста с указанным ID выбирает все попытки и возвращает оценки и ID пользователей выполнивших эти попытки
    *   + Если пользователь преподаватель на курсе
    *   + Если пользователь смотрит свою оценку
    *   - Для остальных
    */
    
    std::cout << "Get user gread:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    nlohmann::json jsonRes;
    jsonRes["users"] = nlohmann::json::array();
    for (int i = 0; i < 5; i++)
    {
        nlohmann::json user;
        user["grade"] = 100;
        user["id"] = i;
        jsonRes["users"].push_back(user);
    }
    
    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Посмотреть ответы пользователя
void GetTestAnswers(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для теста с указанным ID выбирает все попытки и возвращает оценки и ID пользователей выполнивших эти попытки
    *   + Если пользователь преподаватель на курсе
    *   + Если пользователь смотрит свои ответы
    *   - Для остальных
    */
    
    std::cout << "Get test ansvers:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    nlohmann::json jsonRes;
    jsonRes["users"] = nlohmann::json::array();
    for (int i = 0; i < 5; i++)
    {
        nlohmann::json user;
        user["answer"] = nlohmann::json::array();
        for (int j = 5; j < 5; j++)
            user["answer"].push_back(i);
        user["id"] = i;
        jsonRes["users"].push_back(user);
    }

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}

//  Возвращает тест
void GetTestQuests(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает название теста, массив id вопросов
    */

    std::cout << "Add attempt:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    int disid = std::stoi(req.matches[1]);
    int testid = std::stoi(req.matches[2]);
    nlohmann::json jsonRes;
    jsonRes["questions"] = nlohmann::json::array();
    std::vector<int> qid = sql_get_array_int("question_ids", "tests", testid);
    for (auto i : qid)
        jsonRes["questions"].push_back(i);

    res.set_content(jsonRes.dump(), "application/json");


    std::cout << "   End." << std::endl;
}





//      ПОПЫТКИ


//  Создать
void AddAttempt(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если пользователь с ID ещё не отвечал на тест с ID и тест находится в активном состоянии, то создаётся новая попытка и возвращается её ID
    *   + Если пользователь отвечающий на тест
    *   - Для остальных
    */
    
    std::cout << "Add attempt:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO
    
    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);


    std::cout << "   End." << std::endl;
}

//  Изменить
void SetAttempt(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если тест находится в активном состоянии и пользователь ещё не закончил попытку, то для попытки с ID изменяет значение ответа с ID
    *   + Если пользователь отвечающий на тест
    *   - Для остальных
    */
    
    std::cout << "Set attempt:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);


    std::cout << "   End." << std::endl;
}

//  Завершить попытку
void FinAttempt(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если тест находится в активном состоянии и пользователь ещё не закончил попытку, то устанавливает попытку в состояние: завершено.
    *   Если тест переключили в состояние не активный, то все попытки для него автоматически устанавливаются в состояние: завершено
    *   + Если пользователь отвечающий на тест
    *   - Для остальных
    */
    
    std::cout << "Finish attempt:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);


    std::cout << "   End." << std::endl;
}

//  Посмотреть
void GetAttempt(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для пользователя с ID и теста с ID возвращается массив ответов и статус состояние попытки
    *   + Если пользователь преподаватель на курсе
    *   + Если пользователь смотрит свои ответы
    *   - Для остальных
    */
    
    std::cout << "Get attempt:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);


    std::cout << "   End." << std::endl;
}






//      ОТВЕТЫ


//  Создать
void AddAnswer(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Для вопроса с ID создается ответ. Изначально ответ отмечается как -1 (не определённый).
    *   -
    *   Нет разрешения. Ответ автоматически создаётся системой во время создания попытки для каждого вопроса
    */
    
    std::cout << "Add answer:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    int idQest = std::stoi(req.matches[3]);


    std::cout << "   End." << std::endl;
}

//  Посмотреть
void GetAnswer(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Возвращает ID вопроса, и индекс выбранного варианта ответа от 0. Значение -1 - пользователь не дал ответ на вопрос.
    *   + Если пользователь преподаватель на курсе
    *   + Если пользователь смотрит свои ответы
    *   - Для остальных
    */
    
    std::cout << "Get answer:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    int idQest = std::stoi(req.matches[3]);

    nlohmann::json jsonRes;
    jsonRes["id"] = 0;
    jsonRes["answer"] = -1;


    std::cout << "   End." << std::endl;
}

//  Изменить
void ChangeAnswer(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если попытка которой принадлежит ответ не завершена, то изменяет индекс варианта ответа на указанный.
    *   + Если пользователь отвечающий на тест
    *   - Для остальных
    */
    
    std::cout << "Change answer:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    int idQest = std::stoi(req.matches[3]);


    std::cout << "   End." << std::endl;
}

//  Удалить
void DelAnswer(const httplib::Request& req, httplib::Response& res)
{
    /*
    *   Если попытка которой принадлежит ответ не завершена, то изменяет индекс варианта ответа на -1.
    *   + Если пользователь отвечающий на тест
    *   - Для остальных
    */
    
    std::cout << "Del answer:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    int idQest = std::stoi(req.matches[3]);


    std::cout << "   End." << std::endl;
}


