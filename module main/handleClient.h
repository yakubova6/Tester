
//  тут происходит обработка получения и отправки запросов пользователю

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <string>

void handleClient(SOCKET clientSocket);
