
#include "handlerRequest.h"


bool Unauthorized(httplib::Response& res, std::unordered_map<jwt::traits::kazuho_picojson::string_type, jwt::claim> permission)
{
    if (permission.empty()) {
        std::cout << "Token not found or invalid. return status 401" << std::endl;
        res.status = 401; // Unauthorized
        res.set_content("Unauthorized: Token not found or invalid.", "text/plain");
        return true;
    }
    return false;
}



//          ПОЛЬЗОВАТЕЛИ


//  Посмотреть список пользователей
void GetUserList(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user-list:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;
}

//  Посмотреть информацию о пользователе (ФИО)
void GetUserNamea(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get user name:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;
}

//  Изменить ФИО пользователя
void SetUserName(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set user name:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;
}

//  Посмотреть информацию о пользователе (курсы, оценки, тесты)
//  TODO

//  Посмотреть информацию о пользователе (роли)
//  TODO

//  Изменить роли пользователя
//  TODO

//  Посмотреть заблокирован ли пользователь
//  TODO

//  Заблокировать/Разблокировать пользователя
//  TODO



//          ДИСЦИПЛИНЫ


//   Возвращает массив содержащий Название, Описание и ID каждой дисциплины зарегистрированной в системе
void GetDisceplines(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get disceplines:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;
}

//  Возвращает Название, Описание, ID преподавателя для дисциплины по её ID
void GetDisceplineInfo(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get discepline info:" << std::endl;
    
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}

//  Изменить информацию о дисциплине (Название, Описание)
void SetDisceplineInfo(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Set discepline info:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}

//  Посмотреть информацию о дисциплине (Список тестов)
void GetDisceplineTestList(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get discepline tests:" << std::endl;
    
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}

//  Посмотреть информацию о тесте (Активный тест или нет)
//  TODO

//  Активировать/Деактивировать тест
//  TODO

//  Добавить тест в дисциплину по её id
void AddDisceplineTest(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Add discepline test:" << std::endl;
    
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}

//  Удалить тест из дисциплины (id дисциплины и теста)
//  TODO

//  Посмотреть информацию о дисциплине (Список студентов)
void GetDisceplineUserList(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Get discepline user-list:" << std::endl;
    
    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}  

//  Записать пользователя на дисциплину
//  TODO

//  Отчислить пользователя с дисциплины
//  TODO

//  Создать дисциплину
//  TODO

//  Удалить дисциплину
void DelDiscepline(const httplib::Request& req, httplib::Response& res)
{
    std::cout << "Del discepline:" << std::endl;

    auto permission = CheckToken(req);
    if (Unauthorized(res, permission)) return;
    std::cout << "processing..." << std::endl;

    std::string id = req.matches[1];
    std::cout << "id: " << id << std::endl;
}



