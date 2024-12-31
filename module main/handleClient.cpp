
#include "handleClient.h"
#include <iostream>
#include <vector>
#include <thread>

void handleClient(SOCKET clientSocket) {
    // отображение нового подключения на сервере
    std::cout << "New connection" << std::endl;
    std::cout << "Socket: " << clientSocket << std::endl << std::endl;

    // Отправка приветственного сообщения клиенту
    std::string msg = "Welcome to the server of main module!\n";
    int sizeMsg = msg.size();
    send(clientSocket, (char*)&sizeMsg, sizeof(int), NULL);
    send(clientSocket, msg.c_str(), msg.size(), NULL);

    // тут будет код для обработки запросов клиента

    //  ...

    //

    // Закрытие сокета клиента
    closesocket(clientSocket);
}
