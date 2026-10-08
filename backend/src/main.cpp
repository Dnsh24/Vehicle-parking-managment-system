#include "crow.h"
#include "crow/middlewares/cors.h"
#include <pqxx/pqxx>
#include <iostream>
#include <string>

int main()
{
    crow::App<crow::CORSHandler> app;

    auto &cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("Content-Type", "Accept", "Origin")
        .methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
        .origin("*");

    bool database_connected = false;

    try
    {
        pqxx::connection db(
            "host=localhost "
            "port=5432 "
            "dbname=parking_management "
            "user=postgres "
            "password=@24Dinesh");

        std::cout << "Connected to PostgreSQL successfully!" << std::endl;

        pqxx::work txn(db);
        txn.exec("SELECT 1");
        txn.commit();

        database_connected = true;
        std::cout << "Database check successful!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Database connection failed: " << e.what() << std::endl;
    }

    CROW_ROUTE(app, "/api/hello")
    ([]()
     {
        crow::json::wvalue response;
        response["message"] = "C++ backend is working";
        return response; });

    CROW_ROUTE(app, "/api/database")
    ([database_connected]()
     {
        crow::json::wvalue response;
        response["status"] = database_connected ? "success" : "error";
        response["message"] = database_connected
            ? "C++ backend connected to PostgreSQL"
            : "PostgreSQL connection failed. Check the backend logs.";
        response["database"] = "parking_management";
        return response; });

    std::cout << "Server running on http://localhost:3000" << std::endl;

    app.port(3000).multithreaded().run();

    return 0;
}