#pragma once

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "database/database.h"

namespace routes
{

class ReservationRoutes
{
public:
    explicit ReservationRoutes(Database &database);
    void registerRoutes(crow::App<crow::CORSHandler> &app);

private:
    Database &database_;
};

} // namespace routes
