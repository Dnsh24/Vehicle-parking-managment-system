#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/hello")
    ([]()
     {
        crow::json::wvalue response;
        response["message"] = "C++ backend is working";

        return response; });

    app.port(3000).multithreaded().run();

    return 0;
}