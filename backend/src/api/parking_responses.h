#pragma once

#include "crow.h"

#include <cstdint>
#include <string>

namespace api
{
inline crow::json::wvalue areaJson(
    std::int64_t area_id,
    const std::string &name,
    const std::string &location,
    const std::string &operating_hours,
    int capacity,
    const std::string &status,
    std::int64_t available_slots)
{
    crow::json::wvalue area;
    area["area_id"] = area_id;
    area["name"] = name;
    area["location"] = location;
    area["operating_hours"] = operating_hours;
    area["capacity"] = capacity;
    area["status"] = status;
    area["available_slots"] = available_slots;
    return area;
}

inline crow::json::wvalue slotJson(
    std::int64_t slot_id,
    std::int64_t area_id,
    const std::string &slot_number,
    const std::string &vehicle_type,
    const std::string &slot_type,
    bool is_ev,
    const std::string &status)
{
    crow::json::wvalue slot;
    slot["slot_id"] = slot_id;
    slot["area_id"] = area_id;
    slot["slot_number"] = slot_number;
    slot["vehicle_type"] = vehicle_type;
    slot["slot_type"] = slot_type;
    slot["is_ev"] = is_ev;
    slot["status"] = status;
    return slot;
}
}
