#pragma once

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "database/database.h"

#ifdef DELETE
#undef DELETE
#endif

#include <string>

namespace routes
{

class ParkingRoutes
{
public:
    explicit ParkingRoutes(Database &database);
    void registerRoutes(
        crow::App<crow::CORSHandler> &app,
        const DatabaseCheckResult &database_check,
        const std::string &database_name);

private:
    Database &database_;
};

} // namespace routes
