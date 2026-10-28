#include "application.h"

#include <iostream>

ParkingApplication::ParkingApplication(const BackendConfig &config)
    : config_(config),
      database_(config_),
      parking_routes_(database_),
      auth_routes_(database_),
      vehicle_routes_(database_),
      reservation_routes_(database_),
      admin_routes_(database_)
{
}

void ParkingApplication::configureCors()
{
    auto &cors = app_.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("Content-Type, Accept, Origin, Authorization")
        .methods(
            crow::HTTPMethod::GET,
            crow::HTTPMethod::POST,
            crow::HTTPMethod::PUT,
            crow::HTTPMethod::DELETE,
            crow::HTTPMethod::OPTIONS)
        .origin("*");
}

void ParkingApplication::registerRoutes(const DatabaseCheckResult &database_check)
{
    parking_routes_.registerRoutes(app_, database_check, config_.database_name);
    auth_routes_.registerRoutes(app_);
    vehicle_routes_.registerRoutes(app_);
    reservation_routes_.registerRoutes(app_);
    admin_routes_.registerRoutes(app_);
}

void ParkingApplication::run()
{
    configureCors();

    const DatabaseCheckResult database_check = database_.checkConnection();
    if (database_check.connected)
    {
        std::cout << "Database check successful!" << std::endl;
    }
    else
    {
        std::cerr << "Database connection failed: "
                  << database_check.error_message << std::endl;
    }

    registerRoutes(database_check);

    std::cout << "Server running on http://localhost:" << config_.server_port << std::endl;
    app_.port(config_.server_port).multithreaded().run();
}
