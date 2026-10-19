#include "../src/api/errors.h"

#include <cassert>
#include <string>

int main()
{
    const crow::response response = api::errorResponse(
        503,
        "DATABASE_UNAVAILABLE",
        "Database is unavailable");

    assert(response.code == 503);
    assert(response.body.find("\"code\":\"DATABASE_UNAVAILABLE\"") != std::string::npos);
    assert(response.body.find("\"message\":\"Database is unavailable\"") != std::string::npos);
    return 0;
}
