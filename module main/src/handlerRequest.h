
#pragma once

#include "checkToken.h"

#include <httplib.h>
#include <iostream>

#include <jwt-cpp/jwt.h>
#include <string>


//      пользователи
void GetUserList(const httplib::Request& req, httplib::Response& res);              //  Посмотреть список пользователей
void GetUserNamea(const httplib::Request& req, httplib::Response& res);              //  Посмотреть информацию о пользователе (ФИО)
void SetUserName(const httplib::Request& req, httplib::Response& res);              //  Изменить ФИО пользователя
//  Посмотреть информацию о пользователе (курсы, оценки, тесты)
//  Посмотреть информацию о пользователе (роли)
//  Изменить роли пользователя
//  Посмотреть заблокирован ли пользователь
//  Заблокировать/Разблокировать пользователя

//      дисциплины
void GetDisceplines(const httplib::Request& req, httplib::Response& res);           //  Посмотреть список дисциплин
void GetDisceplineInfo(const httplib::Request& req, httplib::Response& res);        //  Посмотреть информацию о дисциплине (Название, Описание, ID преподавателя)
void SetDisceplineInfo(const httplib::Request& req, httplib::Response& res);        //  Изменить информацию о дисциплине (Название, Описание)
void GetDisceplineTestList(const httplib::Request& req, httplib::Response& res);    //  Посмотреть информацию о дисциплине (Список тестов)
//  Посмотреть информацию о тесте (Активный тест или нет)
//  Активировать/Деактивировать тест
void AddDisceplineTest(const httplib::Request& req, httplib::Response& res);        //  Добавить тест в дисциплину по её id
//  Удалить тест из дисциплины (id дисциплины и теста)
void GetDisceplineUserList(const httplib::Request& req, httplib::Response& res);    //  Посмотреть информацию о дисциплине (Список студентов)
//  Записать пользователя на дисциплину
//  Отчислить пользователя с дисциплины
//  Создать дисциплину
void DelDiscepline(const httplib::Request& req, httplib::Response& res);            //  Удалить дисциплину

//      вопросы
//      TODO

//      Тесты
//      TODO

//      Попытки
//      TODO

//      Ответы
//      TODO
