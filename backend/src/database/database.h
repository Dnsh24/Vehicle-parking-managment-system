#pragma once

#include "../config/config.h"

#include <pqxx/pqxx>

#include <string>

struct DatabaseCheckResult
{
    bool connected;
    std::string error_message;
};

class Database
{
public:
    explicit Database(const BackendConfig &config);

    DatabaseCheckResult checkConnection() const;
    pqxx::result executeRead(const std::string &query, const pqxx::params &parameters = {}) const;
    pqxx::result executeWrite(const std::string &query, const pqxx::params &parameters = {}) const;

private:
    pqxx::connection createConnection() const;

    BackendConfig config_;
};
