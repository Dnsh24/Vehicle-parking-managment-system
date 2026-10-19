#include "crow.h"
#include "crow/middlewares/cors.h"
#include "api/errors.h"
#include "config/config.h"
#include "database/database.h"

#include <exception>
#include <iostream>

int main()
{
    BackendConfig config;
    try
    {
        config = BackendConfig::fromEnvironment();
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Configuration error: " << exception.what() << std::endl;
        return 1;
    }

    crow::App<crow::CORSHandler> app;

    auto &cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("Content-Type, Accept, Origin")
        .methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
        .origin("*");

    Database database(config);
    const DatabaseCheckResult database_check = database.checkConnection();

    if (database_check.connected)
    {
        std::cout << "Database check successful!" << std::endl;
    }
    else
    {
        std::cerr << "Database connection failed: "
                  << database_check.error_message << std::endl;
    }

    CROW_ROUTE(app, "/api/hello")
    ([]()
     {
        crow::json::wvalue response;
        response["message"] = "C++ backend is working";
        return response; });

    CROW_ROUTE(app, "/api/database")
    ([database_check, database_name = config.database_name]()
     {
        if (!database_check.connected)
        {
            return api::errorResponse(
                503,
                "DATABASE_UNAVAILABLE",
                "PostgreSQL is unavailable. Check the backend logs.");
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "C++ backend connected to PostgreSQL";
        response["database"] = database_name;
        return crow::response(response); });

    std::cout << "Server running on http://localhost:" << config.server_port << std::endl;

    app.port(config.server_port).multithreaded().run();

    return 0;
}
