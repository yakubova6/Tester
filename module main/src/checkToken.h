
#pragma once

#include <iostream>
#include <string>
#include <chrono>

bool CheckToken(char* message, int msgSize)
{
    std::string secret = "key";

    if (message[0] != 'c')
        return false;
    else
        return true;
}