
#include "handlerRequest.h"


bool Unauthorized(httplib::Response& res, std::unordered_map<jwt::traits::kazuho_picojson::string_type, jwt::claim> permission)
{
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
    
    //   TODO

    //  заглушка

    nlohmann::json jsonRes;
    for (int i = 0; i < 10; i++)
        jsonRes["user" + std::to_string(i)] = i;

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

    //   TODO

    //  заглушка

    int id = std::stoi(req.matches[1]);
    nlohmann::json jsonRes;
    jsonRes["first_name"] = "first_name" + std::to_string(id);      //  имя
    jsonRes["last_name"] = "last_name" + std::to_string(id);        //  фамилия
    jsonRes["middle_name"] = "middle_name" + std::to_string(id);    //  отчество

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
    
    //  TODO

    //  заглушка
    nlohmann::json body = nlohmann::json::parse(req.body);

    std::string first_name = body["first_name"];    //  имя
    std::string last_name = body["last_name"];      //  фамилия
    std::string middle_name = body["middle_name"];  //  отчество

    std::cout << "New full name: " << first_name << ' ' << last_name << ' ' << middle_name << std::endl;


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

    //  TODO

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
        jsonRes.push_back("Disc" + std::to_string(i));

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

    //   TODO

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 5; i++)
        jsonRes.push_back("Role" + std::to_string(i));

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
    std::cout << "User " << std::to_string(id) << " banned." << std::endl; 


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
    std::cout << "User " << std::to_string(id) << " unbanned." << std::endl; 


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
    std::cout << "id: " << id << std::endl;

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

    std::cout << "New name and description: " << name << ' ' << description << std::endl;


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
    std::cout << "id: " << id << std::endl;

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
    std::cout << "Disc: " << idDisc << " Test: " << idTest << std::endl;

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
    std::cout << "Active Disc: " << idDisc << " Test: " << idTest << std::endl;


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
    std::cout << "Deactive Disc: " << idDisc << " Test: " << idTest << std::endl;


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
    std::cout << "id: " << id << std::endl;

    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];

    nlohmann::json jsonRes;
    jsonRes["id"] = 1234;

    std::cout << "Add new test \"" << name << "\" id: " << 1234 << std::endl;
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
    std::cout << "Disc: " << idDisc << " Test: " << idTest << std::endl;


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

    //   TODO

    //  заглушка
    std::string idDisc = req.matches[1];
    std::string idUser = req.matches[2];
    std::cout << "Disc: " << idDisc << " User: " << idUser << std::endl;


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
    std::cout << "Disc: " << idDisc << " User: " << idUser << std::endl;


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

    //   TODO

    //  заглушка
    nlohmann::json body = nlohmann::json::parse(req.body);
    std::string name = body["name"];
    std::string description = body["description"];
    int idPrepod = std::stoi(std::string(body["id"]));

    std::cout << "New discepline: " << name << ' ' << description << ' ' << idPrepod << std::endl;


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
    std::cout << "id: " << id << std::endl;


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


    std::cout << "   End." << std::endl;

    //   TODO
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

    std::string id1 = req.matches[1];
    std::string id2 = req.matches[2];
    std::cout << "idQuest: " << id1 << " idV" << id2 << std::endl;


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


    std::cout << "   End." << std::endl;
}

