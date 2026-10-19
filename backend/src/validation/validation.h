#pragma once

#include <initializer_list>
#include <string>

namespace validation
{
    struct Result
    {
        bool valid;
        std::string message;
    };

    Result requiredString(const std::string &value, const std::string &field);
    Result positiveInteger(const std::string &value, const std::string &field);
    Result email(const std::string &value);
    Result allowedValue(
        const std::string &value,
        const std::string &field,
        std::initializer_list<const char *> allowed_values);
    Result timestamp(const std::string &value, const std::string &field);
}
