#include "crow.h"
#include "crow/middlewares/cors.h"
#include "api/errors.h"
#include "api/parking_responses.h"
#include "config/config.h"
#include "database/database.h"

#include <cstdint>
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

    CROW_ROUTE(app, "/api/parking-areas")
    ([&database](const crow::request &)
     {
        try
        {
            const pqxx::result rows = database.executeRead(
                "SELECT pa.area_id, pa.name, pa.location, "
                "pa.operating_hours, pa.capacity, pa.status, "
                "COUNT(ps.slot_id) FILTER (WHERE ps.status = 'available') AS available_slots "
                "FROM parking_areas pa "
                "LEFT JOIN parking_slots ps ON ps.area_id = pa.area_id "
                "GROUP BY pa.area_id "
                "ORDER BY pa.area_id");

            crow::json::wvalue response;
            response["areas"] = crow::json::wvalue::list();
            std::size_t area_index = 0;
            for (const auto &row : rows)
            {
                response["areas"][area_index++] = api::areaJson(
                    row["area_id"].as<std::int64_t>(),
                    row["name"].c_str(),
                    row["location"].c_str(),
                    row["operating_hours"].is_null() ? "" : row["operating_hours"].c_str(),
                    row["capacity"].as<int>(),
                    row["status"].c_str(),
                    row["available_slots"].as<std::int64_t>());
            }
            return crow::response(response);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Parking-area query failed: " << exception.what() << std::endl;
            return api::errorResponse(
                503,
                "DATABASE_UNAVAILABLE",
                "Parking areas could not be loaded. Check the backend logs.");
        } });

    CROW_ROUTE(app, "/api/parking-areas/<uint>/slots")
    ([&database](unsigned int area_id)
     {
        try
        {
            const pqxx::result area_rows = database.executeRead(
                "SELECT area_id, name FROM parking_areas WHERE area_id = $1",
                pqxx::params{area_id});
            if (area_rows.empty())
            {
                return api::errorResponse(
                    404,
                    "PARKING_AREA_NOT_FOUND",
                    "The requested parking area does not exist.");
            }

            const pqxx::result slot_rows = database.executeRead(
                "SELECT slot_id, area_id, slot_number, vehicle_type, "
                "slot_type, is_ev, status "
                "FROM parking_slots WHERE area_id = $1 ORDER BY slot_number",
                pqxx::params{area_id});

            crow::json::wvalue response;
            response["area"]["area_id"] = area_rows[0]["area_id"].as<std::int64_t>();
            response["area"]["name"] = area_rows[0]["name"].c_str();
            response["slots"] = crow::json::wvalue::list();
            std::size_t slot_index = 0;
            for (const auto &row : slot_rows)
            {
                response["slots"][slot_index++] = api::slotJson(
                    row["slot_id"].as<std::int64_t>(),
                    row["area_id"].as<std::int64_t>(),
                    row["slot_number"].c_str(),
                    row["vehicle_type"].c_str(),
                    row["slot_type"].c_str(),
                    row["is_ev"].as<bool>(),
                    row["status"].c_str());
            }
            return crow::response(response);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Parking-slot query failed: " << exception.what() << std::endl;
            return api::errorResponse(
                503,
                "DATABASE_UNAVAILABLE",
                "Parking slots could not be loaded. Check the backend logs.");
        } });

    std::cout << "Server running on http://localhost:" << config.server_port << std::endl;

    app.port(config.server_port).multithreaded().run();

    return 0;
}
