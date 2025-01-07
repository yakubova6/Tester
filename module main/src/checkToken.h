
#pragma once

#include <chrono>
#include <string>
#include <iostream>
#include <jwt-cpp/jwt.h>
#include <string>
#include <httplib.h>

inline std::string findToken(const httplib::Request& req)
{
    std::string token = "";

    if (req.has_header("Authorization")) {
        std::string authHeader = req.get_header_value("Authorization");

        size_t bearerPos = authHeader.find("Bearer ");
        if (bearerPos != std::string::npos) {
            token = authHeader.substr(bearerPos + 7);
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
    }

    return token;
}

inline std::unordered_map<jwt::traits::kazuho_picojson::string_type, jwt::claim> CheckToken(const httplib::Request& req)
{
    std::unordered_map<jwt::traits::kazuho_picojson::string_type, jwt::claim> payload;
    std::string secret = "key";

    std::cout << "   Find token... ";
    std::string token = findToken(req);

    if (token == "")
    {
        std::cout << "Token not found." << std::endl;
        return payload;
    }
    try {
        auto decoded_token = jwt::decode(token);
        auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256{ secret });
        verifier.verify(decoded_token);
        payload = decoded_token.get_payload_claims();
        std::cout << "Valid token." << std::endl;
    }catch (...) {
        std::cout << "Invalid token." << std::endl;
    }

    return payload;
}
