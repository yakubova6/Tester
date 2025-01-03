

#include "checkToken.h"

Resource CheckToken(SOCKET clientSocket, char* message, int msgSize)
{
    std::string secret = "key";

    beautyPrint(clientSocket, "Find token...");


    beautyPrint(clientSocket, "Token not found.");

    return Resource::Error;
}
