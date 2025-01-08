
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




//          ПОЛЬЗОВАТЕЛИ


//  Посмотреть список пользователей
void GetUserList(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user-list:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    
    //  заглушка
    nlohmann::json jsonRes;
    for (int i = 0; i < 10; i++)
        jsonRes["user" + std::to_string(i)] = i;

    res.set_content(jsonRes.dump(), "application/json");
}

//  Посмотреть информацию о пользователе (ФИО)
void GetUserNamea(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user name:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    //  заглушка
    nlohmann::json jsonRes;
    jsonRes["lastName"] = "lName";
    jsonRes["name"] = "Name";
    jsonRes["middleName"] = "mName";

    res.set_content(jsonRes.dump(), "application/json");
}

//  Изменить ФИО пользователя
void SetUserName(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set user name:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    
}

//  Посмотреть информацию о пользователе (курсы)
void GetUserCourses(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user courses:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть информацию о пользователе (оценки)
void GetUserGrades(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user grades:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть информацию о пользователе (тесты)
void GetUserTests(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user tests:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть информацию о пользователе (роли)
void GetUserRoles(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user roles:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Изменить роли пользователя
void SetUserRoles(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set user roles:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть заблокирован ли пользователь
void GetUserBlock(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user block:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  заблокировать пользователя
void SetUserBlock(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set user block:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  разблокировать пользователя
void SetUserUnblock(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set user unblock:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}






//          ДИСЦИПЛИНЫ


//   Возвращает массив содержащий Название, Описание и ID каждой дисциплины зарегистрированной в системе
void GetDisceplines(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get disceplines:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    
    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
        jsonRes.push_back("Disc" + std::to_string(i));

    res.set_content(jsonRes.dump(), "application/json");
}

//  Возвращает Название, Описание, ID преподавателя для дисциплины по её ID
void GetDisceplineInfo(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get discepline info:" << std::endl;
    
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;

    //  заглушка
    nlohmann::json jsonRes;
    jsonRes["name"] = "Disc" + id;
    jsonRes["description"] = "description....";
    jsonRes["prepod"] = "Prepod" + id;

    res.set_content(jsonRes.dump(), "application/json");
}

//  Изменить информацию о дисциплине (Название, Описание)
void SetDisceplineInfo(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set discepline info:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}

//  Посмотреть информацию о дисциплине (Список тестов)
void GetDisceplineTestList(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get discepline tests:" << std::endl;
    
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
        jsonRes.push_back("Test" + std::to_string(i));

    res.set_content(jsonRes.dump(), "application/json");
}

//  Посмотреть информацию о тесте (Активный тест или нет)
void GetDisceplineTestActive(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get discepline test active:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "Disc: " << idDisc << " Test: " << idTest << std::endl;
}

//  Активировать тест
void SetDisceplineTestActivate(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Activate discepline test:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "Disc: " << idDisc << " Test: " << idTest << std::endl;
}

//  Деактивировать тест
void SetDisceplineTestDeactivate(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Deactivate discepline test:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "Disc: " << idDisc << " Test: " << idTest << std::endl;
}

//  Добавить тест в дисциплину по её id
void AddDisceplineTest(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Add discepline test:" << std::endl;
    
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}

//  Удалить тест из дисциплины (id дисциплины и теста)
void DelDisceplineTest(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Del discepline test:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "Disc: " << idDisc << " Test: " << idTest << std::endl;
}

//  Посмотреть информацию о дисциплине (Список студентов)
void GetDisceplineUserList(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get discepline user-list:" << std::endl;
    
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;

    //  заглушка
    nlohmann::json jsonRes = nlohmann::json::array();
    for (int i = 0; i < 10; i++)
        jsonRes.push_back("User" + std::to_string(i));

    res.set_content(jsonRes.dump(), "application/json");
}  

//  Записать пользователя на дисциплину
void AddDisceplineUser(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Add discepline test user:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "Disc: " << idDisc << " User: " << idTest << std::endl;
}

//  Отчислить пользователя с дисциплины
void DelDisceplineUser(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Del discepline test user:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string idDisc = req.matches[1];
    std::string idTest = req.matches[2];
    std::cout << "Disc: " << idDisc << " User: " << idTest << std::endl;
}

//  Создать дисциплину
void AddDiscepline(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Add discepline:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Удалить дисциплину
void DelDiscepline(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Del discepline:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}






//      ВОПРОСЫ


//  Посмотреть список вопросов
void GetQuestList(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get quest list:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть информацию о вопросе id вопроса и id версии вопроса
void GetQuestInfo(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get quest info by id:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;

    std::string id1 = req.matches[1];
    std::string id2 = req.matches[2];
    std::cout << "idQuest: " << id1 << " idV" << id2 << std::endl;
}

//  Изменить текст вопроса/ответов (создаётся новая версия)
void SetQuestInfo(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set quest info:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Создать вопрос
void AddQuest(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Add quest:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Удалить вопрос
void DelQuest(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Del quest:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}





//      ТЕСТЫ


//  Удалить вопрос из теста
void DelTestQuest(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Del quest from test:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Добавить вопрос в тест
void AddTestQuest(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Add quest to test:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Изменить порядок следования вопросов в тесте
void SetTestQuestSequence(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set quest sequence in test:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть список пользователей прошедших тест
void GetQuestUsers(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get quest users:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть оценку пользователя
void GetTestGreads(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user gread:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть ответы пользователя
void GetTestAnswers(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get test ansvers:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}





//      ПОПЫТКИ


//  Создать
void AddAttempt(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Add attempt:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Изменить
void SetAttempt(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set attempt:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Завершить
void FinAttempt(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Finish attempt:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть
void GetAttempt(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get attempt:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}





//      ОТВЕТЫ


//  Создать
void AddAnswer(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Add answer:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Посмотреть
void GetAnswer(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get answer:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Изменить
void ChangeAnswer(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Change answer:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

//  Удалить
void DelAnswer(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Del answer:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
}

