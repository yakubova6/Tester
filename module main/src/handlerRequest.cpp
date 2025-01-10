
#include "handlerRequest.h"

extern std::string PasswordPostgreSQL = "1234";
extern std::string dbName = "db_module";


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

   //   TODO
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

    //   TODO

    //  заглушка
    nlohmann::json jsonRes;
    jsonRes["banned"] = false;
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

    //   TODO

    //  заглушка
    int id = std::stoi(req.matches[1]);
    std::cout << "   User " << std::to_string(id) << " banned." << std::endl; 


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

    //   TODO

    //  заглушка
    int id = std::stoi(req.matches[1]);
    std::cout << "   User " << std::to_string(id) << " unbanned." << std::endl; 


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

    std::cout << "Get disceplines:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO
    
    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
    {
        nlohmann::json discepline;
        discepline["name"] = "DiscName" + std::to_string(i);
        discepline["description"] = "Description" + std::to_string(i);
        discepline["id"] = i;
        jsonRes.push_back(discepline);
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

    std::cout << "Get discepline info:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id = req.matches[1];
    std::cout << "   id: " << id << std::endl;

    //   TODO

    //  заглушка
    nlohmann::json jsonRes;
    jsonRes["name"] = "Disc" + id;
    jsonRes["description"] = "description....";
    jsonRes["prepod"] = "Prepod" + id;

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

    std::cout << "Set discepline info:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    std::string id = req.matches[1];
    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];
    std::string description = body["description"];

    std::cout << "   New name and description: " << name << ' ' << description << std::endl;


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

    std::string id = req.matches[1];
    std::cout << "   id: " << id << std::endl;

    //   TODO

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
        jsonRes.push_back("Test" + std::to_string(i));

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

    //   TODO

    //  заглушка
    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "   Disc: " << idDisc << " Test: " << idTest << std::endl;

    nlohmann::json jsonRes;
    jsonRes["active"] = false;

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

    //   TODO

    //  заглушка
    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "   Active Disc: " << idDisc << " Test: " << idTest << std::endl;


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

    //   TODO

    //  заглушка
    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "   Deactive Disc: " << idDisc << " Test: " << idTest << std::endl;


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

    //  заглушка
    std::string id = req.matches[1];
    std::cout << "   id: " << id << std::endl;

    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];

    nlohmann::json jsonRes;
    jsonRes["id"] = 1234;

    std::cout << "   Add new test \"" << name << "\" id: " << 1234 << std::endl;
    res.set_content(jsonRes.dump(), "application/json");


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

    //   TODO

    //  заглушка
    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "   Disc: " << idDisc << " Test: " << idTest << std::endl;


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
    
    std::cout << "Get discepline user-list:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
        jsonRes.push_back("User" + std::to_string(i));

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
    
    std::cout << "Add discepline test user:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string idDisc = req.matches[1];
    std::string idUser = req.matches[2];
    std::cout << "   Disc: " << idDisc << " User: " << idUser << std::endl;

     try {
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");
        pqxx::work txn(conn);

        auto result = txn.exec_params1(
            "SELECT $1 = ANY(disciplines) FROM users WHERE id = $2",
            idDisc, idUser
        );
        if (!result[0].is_null()) {
            std::cout << "   Discipline already exists for this user!" << std::endl;
            return;
        }
        txn.exec_params(
            "UPDATE users "
            "SET disciplines = array_append(disciplines, $1) "
            "WHERE id = $2",
            idDisc, idUser
        );
        txn.commit();
    } catch (const std::exception &e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }


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
    
    std::cout << "Del discepline test user:" << std::endl;
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //   TODO

    //  заглушка
    std::string idDisc = req.matches[1];
    std::string idUser = req.matches[2];
    std::cout << "   Disc: " << idDisc << " User: " << idUser << std::endl;


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
        pqxx::connection conn("dbname=" + dbName + " user=postgres password=" + PasswordPostgreSQL + " host=127.0.0.1 port=5432");

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

    //   TODO

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
    {
        nlohmann::json discepline;
        discepline["name"] = "quest name" + std::to_string(i);
        discepline["version"] = std::to_string(i);
        discepline["idAuthor"] = i;
        jsonRes.push_back(discepline);
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

    //   TODO

    //  заглушка
    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];
    std::string description = body["description"];
    std::vector<std::string> options;
    for (const auto i : body["options"])
        options.push_back(i);
    int correct_answer = body["correct_answer"];


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

    //   TODO

    //  заглушка
    int idDisc = std::stoi(req.matches[1]);
    int idTest = std::stoi(req.matches[2]);
    int idQuest = std::stoi(req.matches[3]);


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

