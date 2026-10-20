#pragma once

#include <string>

namespace auth
{
std::string hashPassword(const std::string &password);
bool verifyPassword(const std::string &password, const std::string &encoded_hash);
std::string createToken();
std::string hashToken(const std::string &token);
}
