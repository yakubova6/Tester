
//  вот почему я не изучил как вообще можно работать с этим?
//  теперь приходится переписывать весь код с нуля...
//  сейчас 1:40. чувствую это будет весёлая ночка

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <string>
#include <iostream>

#include "handlerRequest.h"


int main ()
{
    httplib::Server server;

//      пользователи
    server.Get("/api/db/users", GetUserList);                                   //  Посмотреть список пользователей
    server.Get("/api/db/users/(\\d+)/name", GetUserNamea);                      //  Посмотреть информацию о пользователе (ФИО)
    server.Put("/api/db/users/(\\d+)/name", SetUserName);                       //  Изменить ФИО пользователя
    server.Get("/api/db/users/(\\d+)/courses", GetUserCourses);                 //  Посмотреть информацию о пользователе (курсы)
    server.Get("/api/db/users/(\\d+)/grades", GetUserGrades);                   //  Посмотреть информацию о пользователе (оценки)
    server.Get("/api/db/users/(\\d+)/tests", GetUserTests);                     //  Посмотреть информацию о пользователе (тесты)
    server.Get("/api/db/users/(\\d+)/roles", GetUserRoles);                     //  Посмотреть информацию о пользователе (роли)
    server.Put("/api/db/users/(\\d+)/roles", SetUserRoles);                     //  Изменить роли пользователя    
    server.Get("/api/db/users/(\\d+)/block", GetUserBlock);                     //  Посмотреть заблокирован ли пользователь
    server.Put("/api/db/users/(\\d+)/block", SetUserBlock);                     //  заблокировать пользователя
    server.Put("/api/db/users/(\\d+)/unblock", SetUserUnblock);                 //  разблокировать пользователя
// ..............................

//      дисциплины
    server.Get("/api/db/disciplines", GetDisceplines);                                          //  Посмотреть список дисциплин
    server.Get("/api/db/disciplines/(\\d+)", GetDisceplineInfo);                                //  Посмотреть информацию о дисциплине (Название, Описание, ID преподавателя)
    server.Put("/api/db/disciplines/(\\d+)", SetDisceplineInfo);                                //  Изменить информацию о дисциплине (Название, Описание)
    server.Get("/api/db/disciplines/(\\d+)/tests", GetDisceplineTestList);                      //  Посмотреть информацию о дисциплине (Список тестов) по её id
    server.Get("/api/db/disciplines/(\\d+)tests/(\\d+)/active", GetDisceplineTestActive);       //  Посмотреть информацию о тесте (Активный тест или нет) (id теста и дисциплины)
    server.Put("/api/db/disciplines/(\\d+)tests/(\\d+)/activate", SetDisceplineTestActivate);   //  Активировать тест
    server.Put("/api/db/disciplines/(\\d+)tests/(\\d+)/deactivate", SetDisceplineTestDeactivate);//  Деактивировать тест
    server.Post("/api/db/disciplines/(\\d+)tests", AddDisceplineTest);                          //  Добавить тест в дисциплину по её id
    server.Delete("/api/db/disciplines/(\\d+)/tests/(\\d+)", DelDisceplineTest);                //  Удалить тест из дисциплины (id дисциплины и теста)
    server.Get("/api/db/disciplines(\\d+)/users", GetDisceplineUserList);                       //  Посмотреть информацию о дисциплине (Список студентов)    
    server.Put("/api/db/disciplines(\\d+)/users(\\d+)", AddDisceplineUser);                     //  Записать пользователя на дисциплину
    server.Delete("/api/db/disciplines(\\d+)/users(\\d+)", DelDisceplineUser);                  //  Отчислить пользователя с дисциплины
    server.Post("/api/db/disciplines", AddDiscepline);                                          //  Создать дисциплину
    server.Delete("/api/db/disciplines/(\\d+)", DelDiscepline);                                 //  Удалить дисциплину
// ..............................

//      вопросы
//  TODO                                                                        //  Посмотреть список вопросов
//  TODO                                                                        //  Посмотреть информацию о вопросе
//  TODO                                                                        //  Изменить текст вопроса/ответов (создаётся новая версия)
//  TODO                                                                        //  Создать вопрос
//  TODO                                                                        //  Удалить вопрос
//  ..............................

//      тесты
//  TODO                                                                        //  Удалить вопрос из теста
//  TODO                                                                        //  Добавить вопрос в тест
//  TODO                                                                        //  Изменить порядок следования вопросов в тесте
//  TODO                                                                        //  Посмотреть список пользователей прошедших тест
//  TODO                                                                        //  Посмотреть оценку пользователя
//  TODO                                                                        //  Посмотреть ответы пользователя
//  ..............................

//      попытки
//  TODO                                                                        //  Создать
//  TODO                                                                        //  Изменить
//  TODO                                                                        //  Завершить попытку
//  TODO                                                                        //  Посмотреть попытку
//  ..............................

//      ответы
//  TODO                                                                        //  Создать
//  TODO                                                                        //  Посмотреть
//  TODO                                                                        //  Изменить
//  TODO                                                                        //  Удалить
//  ..............................


    std::cout << "Server started at http://127.0.0.1:1111/" << std::endl;
    server.listen("127.0.0.1", 1111);
    std::cout << "Server was stoped." << std::endl;

    return 0;
}
