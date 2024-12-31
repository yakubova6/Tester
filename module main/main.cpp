
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <vector>
#include <string>
#include <thread>
#include <iostream>

#define ADRES "127.0.0.1"
#define PORT  1111



// ФУНКЦИЯ ДЛЯ ОБРАБОТКИ ПОДКЛЮЧЕНИЯ

void handleClient(SOCKET clientSocket) {
	std::cout << "Client connected!" << std::endl;

	// Отправка сообщения клиенту
	std::string msg = "Welcome to the server!\n";
	int sizeMsg = msg.size();
	send(clientSocket, (char*)&sizeMsg, sizeof(int), NULL);
	send(clientSocket, msg.c_str(), msg.size(), NULL);

	system("pause");

	// Закрытие сокета клиента
	closesocket(clientSocket);
}



int main()
{

	//		ЗАГРУЗКА СЕТЕВОЙ БИБЛИОТЕКИ

	std::cout << "Load lib...       ";

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "Error.\n";
		return 1;
	} else {
		std::cout << "Done.\n";
	}



	//		СОЗДАНИЕ СОКЕТА СЕРВЕРА

	std::cout << "Create socket...  ";

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Error.\n";
		WSACleanup();
		return 1;
	} else {
		std::cout << "Done.\n";
	}



	//		ПРИСВАИВАНИЕ АДРЕСА И ПОРТА
	
	std::cout << "Adres and port... ";

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(ADRES);
	serverAddr.sin_port = htons(PORT);

	std::cout << "Done.\n";



	//		ПРИВЯЗКА СОКЕКА К ПОРТУ И АДРЕСУ

	bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));



	//		ПРОСЛУШИВАНИЕ ПОРТА

	listen(serverSocket, SOMAXCONN);

	std::cout << "\nServer started...\n\n";



	//		ОБРАБОТКА ПОПЫТКИ ПОДКЛЮЧЕНИЯ
	
	std::vector<std::thread> clientThreads;

	while (true)
	{
		sockaddr_in clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
			continue;
		}

		// Запуск нового потока для обработки клиента
		clientThreads.emplace_back(std::thread(handleClient, clientSocket));
	}

	return 0;
}


