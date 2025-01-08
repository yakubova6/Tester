
#pragma once

#include "checkToken.h"

#include <httplib.h>
#include <iostream>

#include <jwt-cpp/jwt.h>
#include <string>
#include <nlohmann/json.hpp>

//      пользователи
void GetUserList(const httplib::Request& req, httplib::Response& res);              //  Посмотреть список пользователей
void GetUserNamea(const httplib::Request& req, httplib::Response& res);             //  Посмотреть информацию о пользователе (ФИО)
void SetUserName(const httplib::Request& req, httplib::Response& res);              //  Изменить ФИО пользователя
void GetUserCourses(const httplib::Request& req, httplib::Response& res);           //  Посмотреть информацию о пользователе (курсы)              
void GetUserGrades(const httplib::Request& req, httplib::Response& res);            //  Посмотреть информацию о пользователе (оценки)           
void GetUserTests(const httplib::Request& req, httplib::Response& res);             //  Посмотреть информацию о пользователе (тесты)           
void GetUserRoles(const httplib::Request& req, httplib::Response& res);             //  Посмотреть информацию о пользователе (роли)                  
void SetUserRoles(const httplib::Request& req, httplib::Response& res);             //  Изменить роли пользователя              
void GetUserBlock(const httplib::Request& req, httplib::Response& res);             //  Посмотреть заблокирован ли пользователь               
void SetUserBlock(const httplib::Request& req, httplib::Response& res);             //  заблокировать пользователя
void SetUserUnblock(const httplib::Request& req, httplib::Response& res);           //  разблокировать пользователя

//      дисциплины
void GetDisceplines(const httplib::Request& req, httplib::Response& res);               //  Посмотреть список дисциплин
void GetDisceplineInfo(const httplib::Request& req, httplib::Response& res);            //  Посмотреть информацию о дисциплине (Название, Описание, ID преподавателя)
void SetDisceplineInfo(const httplib::Request& req, httplib::Response& res);            //  Изменить информацию о дисциплине (Название, Описание)
void GetDisceplineTestList(const httplib::Request& req, httplib::Response& res);        //  Посмотреть информацию о дисциплине (Список тестов)
void GetDisceplineTestActive(const httplib::Request& req, httplib::Response& res);      //  Посмотреть информацию о тесте (Активный тест или нет)
void SetDisceplineTestActivate(const httplib::Request& req, httplib::Response& res);    //  Активировать/Деактивировать тест
void SetDisceplineTestDeactivate(const httplib::Request& req, httplib::Response& res);  //  Активировать/Деактивировать тест
void AddDisceplineTest(const httplib::Request& req, httplib::Response& res);            //  Добавить тест в дисциплину по её id
void DelDisceplineTest(const httplib::Request& req, httplib::Response& res);            //  Удалить тест из дисциплины (id дисциплины и теста)
void GetDisceplineUserList(const httplib::Request& req, httplib::Response& res);        //  Посмотреть информацию о дисциплине (Список студентов)
void AddDisceplineUser(const httplib::Request& req, httplib::Response& res);            //  Записать пользователя на дисциплину
void DelDisceplineUser(const httplib::Request& req, httplib::Response& res);            //  Отчислить пользователя с дисциплины
void AddDiscepline(const httplib::Request& req, httplib::Response& res);                //  Создать дисциплину
void DelDiscepline(const httplib::Request& req, httplib::Response& res);                //  Удалить дисциплину


//      вопросы
//      TODO

//      Тесты
//      TODO

//      Попытки
//      TODO

//      Ответы
//      TODO
