
//  тут происходит обработка получения и отправки запросов пользователю

#pragma once

//#include "checkToken.h"

#include <jwt-cpp/jwt.h>
#include <chrono>
#include <string>
#include <iostream>

#include "simpleFunctions.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <string>
#include <vector>
#include <thread>

void handleClient(SOCKET clientSocket);
