#include "../src/api/errors.h"
#include "../src/api/parking_responses.h"

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

    const std::string area = api::areaJson(
        1,
        "Mall Central — Ground Floor",
        "Mall Central",
        "07:00 - 22:00",
        32,
        "active",
        12).dump();
    assert(area.find("\"area_id\":1") != std::string::npos);
    assert(area.find("\"capacity\":32") != std::string::npos);
    assert(area.find("\"available_slots\":12") != std::string::npos);

    const std::string slot = api::slotJson(
        10,
        1,
        "G-A01",
        "car",
        "standard",
        false,
        "available").dump();
    assert(slot.find("\"slot_number\":\"G-A01\"") != std::string::npos);
    assert(slot.find("\"status\":\"available\"") != std::string::npos);
    return 0;
}
