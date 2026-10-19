#include "database.h"

#include <exception>
#include <string>

namespace
{
std::string quoteConnectionValue(const std::string &value)
{
    std::string quoted_value = "'";
    for (const char character : value)
    {
        if (character == '\\' || character == '\'')
        {
            quoted_value += '\\';
        }
        quoted_value += character;
    }
    quoted_value += '\'';
    return quoted_value;
}
}

Database::Database(const BackendConfig &config)
    : config_(config)
{
}

pqxx::connection Database::createConnection() const
{
    return pqxx::connection(
        "host=" + quoteConnectionValue(config_.database_host) +
        " port=" + quoteConnectionValue(config_.database_port) +
        " dbname=" + quoteConnectionValue(config_.database_name) +
        " user=" + quoteConnectionValue(config_.database_user) +
        " password=" + quoteConnectionValue(config_.database_password));
}

pqxx::result Database::executeRead(
    const std::string &query,
    const pqxx::params &parameters) const
{
    pqxx::connection connection = createConnection();
    pqxx::read_transaction transaction(connection);
    return transaction.exec(query, parameters);
}

pqxx::result Database::executeWrite(
    const std::string &query,
    const pqxx::params &parameters) const
{
    pqxx::connection connection = createConnection();
    pqxx::work transaction(connection);
    pqxx::result result = transaction.exec(query, parameters);
    transaction.commit();
    return result;
}

DatabaseCheckResult Database::checkConnection() const
{
    try
    {
        executeRead("SELECT 1");
        return {true, {}};
    }
    catch (const std::exception &exception)
    {
        return {false, exception.what()};
    }
}
