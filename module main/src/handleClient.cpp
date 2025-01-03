
#include "handleClient.h"

#include <iostream>
#include <vector>
#include <thread>
#include <iomanip>

#include "checkToken.h"


void handleClient(SOCKET clientSocket) {
    // отображение нового подключения на сервере
    std::cout << "New connection on socket " << clientSocket << std::endl;

    //  ожидается JWT токен

    const int msgSize = 1024;
    char* message = new char[msgSize];;

    while (true)
    {
        recv(clientSocket, message, msgSize, NULL);

        std::cout << '[' << std::setfill(' ') << std::setw(7) << clientSocket << "] " << message << std::endl;
    
        //  попытка обработать сообщение
        if (!CheckToken(message, msgSize))
        {
            // Закрытие сокета клиента
            std::cout << '[' << std::setfill(' ') << std::setw(7) << clientSocket << "] Invalid request received. Client disconnected\n";
            closesocket(clientSocket);
            break;
        }
        else
        {
            std::cout << '[' << std::setfill(' ') << std::setw(7) << clientSocket << "] Request accepted. Executing request...\n";
        }
    }    
}
