

#include "handleClient.h"

inline std::string findToken(std::string message);
std::unordered_map<jwt::traits::kazuho_picojson::string_type, jwt::claim> CheckToken(SOCKET clientSocket, char* message, int msgSize);

void handleClient(SOCKET clientSocket) {
    // отображение нового подключения на сервере
    std::cout << "New connection on socket " << clientSocket << std::endl;

    const int msgSize = 2048;
    char* message = new char[msgSize];

    while (true)
    {
        //      получение сообщения и проверка что пользователь всё ещё подключен
        int bytesReceived = recv(clientSocket, message, 2048, NULL);
        if (bytesReceived == 0 || bytesReceived == -1) {
            beautyPrint(clientSocket, "Client has disconnected or a connection error has occurred.");
            break;
        }

        //      полученное сообщение
        beautyPrint(clientSocket, "Message : " +  std::string(message));
    
        //      проверка токена
        auto permission = CheckToken(clientSocket, message, msgSize);
        if (permission.empty())
        {
            beautyPrint(clientSocket, "Error: 401");
            beautyPrint(clientSocket, "Send code 401");
            sendToUser(clientSocket, 401, "Access Denied");
            closesocket(clientSocket);
            beautyPrint(clientSocket, "Socket closed.");
            continue;
        }
        beautyPrint(clientSocket, "Token correct: 200");
        beautyPrint(clientSocket, "Send code 200");
        sendToUser(clientSocket, 200, "TODO");
        closesocket(clientSocket);
        beautyPrint(clientSocket, "Socket closed.");


        //      TODO
        //
        //      1.
        //      дописать вытаскивание конкретно запроса из запроса
        //      тавтология какая-то
        //      короче получить действие которое нужно выполнить
        //
        //      2.
        //      подключить PostgreSQL
        //
        //      3.
        //      написать функции для выполнения действий из п.1
        //      если функция специфичекая проверить доступность действия из permission


    }
}



inline std::string findToken(std::string message)
{
    std::string token = "";

    size_t bearerPos = message.find("Bearer ");
    if (bearerPos != std::string::npos) {
        token = message.substr(bearerPos + 7);
    }

    size_t cut_pos = token.size();
    if (token.find(' ') != std::string::npos) {
        if (cut_pos > token.find(' '))
            cut_pos = token.find(' ');
    }
    if (token.find(' ') != std::string::npos) {
        if (cut_pos > token.find('\n'))
            cut_pos = token.find('\n');
    }
    if (token.find(' ') != std::string::npos) {
        if (cut_pos > token.find('\r'))
            cut_pos = token.find('\r');
    }

    token = token.substr(0, cut_pos);

    return token;
}



std::unordered_map<jwt::traits::kazuho_picojson::string_type, jwt::claim> CheckToken(SOCKET clientSocket, char* message, int msgSize)
{
    std::unordered_map<jwt::traits::kazuho_picojson::string_type, jwt::claim> payload;
    std::string secret = "key";

    beautyPrint(clientSocket, "Find token...");
    std::string token = findToken(message);

    if (token == "")
    {
        beautyPrint(clientSocket, "Token not found.");
        return payload;
    }

    // Выполняем проверку подписи токена ключом SECRET
    jwt::decoded_jwt<jwt::traits::kazuho_picojson> decoded_token = jwt::decode(token);
    try {
        jwt::verifier<jwt::default_clock, jwt::traits::kazuho_picojson> verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256{ secret });
        verifier.verify(decoded_token);
        // Если токен валидный, проходим дальше иначе попадаем в catch
        payload = decoded_token.get_payload_claims();
    }catch (...) {
        beautyPrint(clientSocket, "Invalid token.");
    }

    return payload;
}
