#include "crow.h"
#include <pqxx/pqxx>
#include <iostream>
#include <string>

int main()
{
    crow::SimpleApp app;

    // PostgreSQL test
    try
    {
        pqxx::connection db(
            "host=localhost "
            "port=5432 "
            "dbname=parking_management "
            "user=postgres "
            "password=@24Dinesh");

        std::cout << "Connected to PostgreSQL successfully!" << std::endl;

        pqxx::transaction<> transaction(db);

        pqxx::result result = transaction.exec(
            "SELECT current_database();");

        std::cout << "Database: "
                  << result[0][0].as<std::string>()
                  << std::endl;

        transaction.commit();

        std::cout << "Database query successful!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Database error: "
                  << e.what()
                  << std::endl;

        return 1;
    }

    // PostgreSQL test API
    CROW_ROUTE(app, "/api/database")
    ([]()
     {
        crow::json::wvalue response;

        response["status"] = "success";
        response["message"] = "C++ backend is connected to PostgreSQL";
        response["database"] = "vehicle_parking";

        return response; });

    // Basic backend test
    CROW_ROUTE(app, "/api/hello")
    ([]()
     {
        crow::json::wvalue response;

        response["message"] = "C++ backend is working";

        return response; });

    std::cout << "Server running on http://localhost:3000" << std::endl;

    app.port(3000).multithreaded().run();

    return 0;
}