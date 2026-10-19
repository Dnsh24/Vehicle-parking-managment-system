#pragma once

#include <string>

struct BackendConfig
{
    std::string database_host;
    std::string database_port;
    std::string database_name;
    std::string database_user;
    std::string database_password;
    unsigned short server_port;

    static BackendConfig fromEnvironment();
};
