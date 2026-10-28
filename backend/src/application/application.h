#pragma once

#include "config/config.h"
#include "crow.h"
#include "crow/middlewares/cors.h"
#include "database/database.h"
#include "routes/admin_routes.h"
#include "routes/auth_routes.h"
#include "routes/parking_routes.h"
#include "routes/reservation_routes.h"
#include "routes/vehicle_routes.h"

class ParkingApplication
{
public:
    explicit ParkingApplication(const BackendConfig &config);
    void run();

private:
    void configureCors();
    void registerRoutes(const DatabaseCheckResult &database_check);

    BackendConfig config_;
    Database database_;
    crow::App<crow::CORSHandler> app_;
    routes::ParkingRoutes parking_routes_;
    routes::AuthRoutes auth_routes_;
    routes::VehicleRoutes vehicle_routes_;
    routes::ReservationRoutes reservation_routes_;
    routes::AdminRoutes admin_routes_;
};
