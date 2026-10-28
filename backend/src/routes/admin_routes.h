#pragma once

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "database/database.h"

#ifdef DELETE
#undef DELETE
#endif

namespace routes
{

class AdminRoutes
{
public:
    explicit AdminRoutes(Database &database);
    void registerRoutes(crow::App<crow::CORSHandler> &app);

private:
    Database &database_;
};

} // namespace routes
