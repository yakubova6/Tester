
#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include <iomanip>
#include <winsock2.h>
#include <string>
#include <iostream>
#include <sstream>


inline void beautyPrint(SOCKET soc, std::string text)
{
    std::cout << '[' << std::setfill(' ') << std::setw(7) << soc << "] " << text << std::endl;
}

inline void sendToUser(SOCKET clientSocket, int code, std::string message)
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << code << " Forbidden\r\n"
                   << "\r\n"
                   << message;

    // Преобразуем поток в строку
    std::string httpResponse = responseStream.str();

    // Отправляем ответ клиенту
    send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
}

