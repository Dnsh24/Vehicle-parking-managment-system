#include "validation.h"

#include <cctype>
#include <regex>
#include <stdexcept>

namespace
{
bool isWhitespaceOnly(const std::string &value)
{
    for (const unsigned char character : value)
    {
        if (!std::isspace(character))
        {
            return false;
        }
    }
    return true;
}

validation::Result invalid(const std::string &message)
{
    return {false, message};
}
}

namespace validation
{
Result requiredString(const std::string &value, const std::string &field)
{
    if (value.empty() || isWhitespaceOnly(value))
    {
        return invalid(field + " is required");
    }
    return {true, {}};
}

Result positiveInteger(const std::string &value, const std::string &field)
{
    if (value.empty())
    {
        return invalid(field + " is required");
    }

    try
    {
        std::size_t parsed_characters = 0;
        const long long number = std::stoll(value, &parsed_characters);
        if (parsed_characters != value.size() || number <= 0)
        {
            return invalid(field + " must be a positive integer");
        }
    }
    catch (const std::exception &)
    {
        return invalid(field + " must be a positive integer");
    }

    return {true, {}};
}

Result email(const std::string &value)
{
    static const std::regex pattern(
        R"(^[A-Za-z0-9.!#$%&'*+/=?^_`{|}~-]+@[A-Za-z0-9-]+(\.[A-Za-z0-9-]+)+$)");

    if (!std::regex_match(value, pattern))
    {
        return invalid("email must be a valid email address");
    }
    return {true, {}};
}

Result allowedValue(
    const std::string &value,
    const std::string &field,
    std::initializer_list<const char *> allowed_values)
{
    for (const char *allowed : allowed_values)
    {
        if (value == allowed)
        {
            return {true, {}};
        }
    }

    return invalid(field + " has an unsupported value");
}

Result timestamp(const std::string &value, const std::string &field)
{
    static const std::regex pattern(
        R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(Z|[+-]\d{2}:\d{2})$)");

    if (!std::regex_match(value, pattern))
    {
        return invalid(field + " must be an ISO-8601 timestamp");
    }
    return {true, {}};
}
}
