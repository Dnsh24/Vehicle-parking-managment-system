#pragma once

#include "crow.h"

#include <string>

namespace api
{
inline crow::response errorResponse(
    int status_code,
    const std::string &code,
    const std::string &message)
{
    crow::json::wvalue response;
    response["error"]["code"] = code;
    response["error"]["message"] = message;

    return crow::response(status_code, response);
}
}
