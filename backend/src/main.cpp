#include "crow.h"
#include "crow/middlewares/cors.h"
#include "api/errors.h"
#include "api/parking_responses.h"
#include "auth/auth.h"
#include "config/config.h"
#include "database/database.h"
#include "validation/validation.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <string>

#ifdef DELETE
#undef DELETE
#endif

namespace
{
struct AuthenticatedUser
{
    std::int64_t user_id;
    std::string name;
    std::string email;
    std::string phone;
    std::string role;
};

std::string lowerEmail(std::string email)
{
    std::transform(email.begin(), email.end(), email.begin(), [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });
    return email;
}

std::string jsonString(const crow::json::rvalue &body, const char *key)
{
    if (body.t() != crow::json::type::Object ||
        !body.has(key) ||
        body[key].t() != crow::json::type::String)
    {
        return {};
    }
    return body[key].s();
}

crow::json::wvalue userJson(const AuthenticatedUser &user)
{
    crow::json::wvalue response;
    response["user_id"] = user.user_id;
    response["name"] = user.name;
    response["email"] = user.email;
    response["phone"] = user.phone;
    response["role"] = user.role;
    return response;
}

std::optional<AuthenticatedUser> findAuthenticatedUser(
    const Database &database,
    const crow::request &request)
{
    const std::string authorization = request.get_header_value("Authorization");
    constexpr const char *prefix = "Bearer ";
    if (authorization.rfind(prefix, 0) != 0 || authorization.size() <= 7)
    {
        return std::nullopt;
    }

    const pqxx::result rows = database.executeRead(
        "SELECT u.user_id, u.name, u.email, COALESCE(u.phone, '') AS phone, u.role "
        "FROM auth_sessions s "
        "JOIN users u ON u.user_id = s.user_id "
        "WHERE s.token_hash = $1 AND s.expires_at > CURRENT_TIMESTAMP "
        "AND u.is_active = TRUE",
        pqxx::params{auth::hashToken(authorization.substr(7))});
    if (rows.empty())
    {
        return std::nullopt;
    }

    return AuthenticatedUser{
        rows[0]["user_id"].as<std::int64_t>(),
        rows[0]["name"].c_str(),
        rows[0]["email"].c_str(),
        rows[0]["phone"].c_str(),
        rows[0]["role"].c_str()};
}

std::string createSession(const Database &database, std::int64_t user_id)
{
    const std::string token = auth::createToken();
    database.executeWrite(
        "DELETE FROM auth_sessions WHERE expires_at <= CURRENT_TIMESTAMP",
        pqxx::params{});
    database.executeWrite(
        "INSERT INTO auth_sessions (user_id, token_hash, expires_at) "
        "VALUES ($1, $2, CURRENT_TIMESTAMP + INTERVAL '7 days')",
        pqxx::params{user_id, auth::hashToken(token)});
    return token;
}

crow::response unauthorized()
{
    return api::errorResponse(401, "UNAUTHENTICATED", "A valid login session is required.");
}

}

int main()
{
    BackendConfig config;
    try
    {
        config = BackendConfig::fromEnvironment();
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Configuration error: " << exception.what() << std::endl;
        return 1;
    }

    crow::App<crow::CORSHandler> app;

    auto &cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("Content-Type, Accept, Origin, Authorization")
        .methods(
            crow::HTTPMethod::GET,
            crow::HTTPMethod::POST,
            crow::HTTPMethod::PUT,
            crow::HTTPMethod::DELETE,
            crow::HTTPMethod::OPTIONS)
        .origin("*");

    Database database(config);
    const DatabaseCheckResult database_check = database.checkConnection();

    if (database_check.connected)
    {
        std::cout << "Database check successful!" << std::endl;
    }
    else
    {
        std::cerr << "Database connection failed: "
                  << database_check.error_message << std::endl;
    }

    CROW_ROUTE(app, "/api/hello")
    ([]()
     {
        crow::json::wvalue response;
        response["message"] = "C++ backend is working";
        return response; });

    CROW_ROUTE(app, "/api/database")
    ([database_check, database_name = config.database_name]()
     {
        if (!database_check.connected)
        {
            return api::errorResponse(
                503,
                "DATABASE_UNAVAILABLE",
                "PostgreSQL is unavailable. Check the backend logs.");
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "C++ backend connected to PostgreSQL";
        response["database"] = database_name;
        return crow::response(response); });

    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::POST)
    ([&database](const crow::request &request)
     {
        try
        {
            const auto body = crow::json::load(request.body);
            const std::string username = jsonString(body, "username");
            const std::string provided_name = jsonString(body, "name");
            const std::string name = provided_name.empty() ? username : provided_name;
            const std::string email = lowerEmail(jsonString(body, "email"));
            const std::string password = jsonString(body, "password");
            const std::string phone = jsonString(body, "phone");
            if (!validation::requiredString(username, "username").valid ||
                username.size() > 50)
            {
                return api::errorResponse(400, "INVALID_USERNAME", "Username is required and must be at most 50 characters.");
            }
            if (!validation::email(email).valid)
            {
                return api::errorResponse(400, "INVALID_EMAIL", "Email must be a valid email address.");
            }
            if (password.size() < 8)
            {
                return api::errorResponse(400, "INVALID_PASSWORD", "Password must be at least 8 characters.");
            }
            const pqxx::result rows = database.executeWrite(
                "INSERT INTO users (username, name, email, password_hash, phone) "
                "VALUES ($1, $2, $3, $4, NULLIF($5, '')) "
                "RETURNING user_id, name, email, COALESCE(phone, '') AS phone, role",
                pqxx::params{username, name, email, auth::hashPassword(password), phone});
            const AuthenticatedUser user{
                rows[0]["user_id"].as<std::int64_t>(),
                rows[0]["name"].c_str(),
                rows[0]["email"].c_str(),
                rows[0]["phone"].c_str(),
                rows[0]["role"].c_str()};
            crow::json::wvalue response;
            response["user"] = userJson(user);
            response["token"] = createSession(database, user.user_id);
            return crow::response(201, response);
        }
        catch (const pqxx::unique_violation &)
        {
            return api::errorResponse(409, "ACCOUNT_IDENTIFIER_ALREADY_REGISTERED", "That username or email is already registered.");
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Registration failed: " << exception.what() << std::endl;
            return api::errorResponse(503, "DATABASE_UNAVAILABLE", "The account could not be created.");
        } });

    CROW_ROUTE(app, "/api/auth/login").methods(crow::HTTPMethod::POST)
    ([&database](const crow::request &request)
     {
        try
        {
            const auto body = crow::json::load(request.body);
            const std::string email = lowerEmail(jsonString(body, "email"));
            const std::string password = jsonString(body, "password");
            const pqxx::result rows = database.executeRead(
                "SELECT user_id, name, email, password_hash, COALESCE(phone, '') AS phone, role, is_active "
                "FROM users WHERE LOWER(email) = LOWER($1) OR LOWER(username) = LOWER($1)",
                pqxx::params{email});
            if (rows.empty() ||
                !rows[0]["is_active"].as<bool>() ||
                !auth::verifyPassword(password, rows[0]["password_hash"].c_str()))
            {
                return api::errorResponse(401, "INVALID_CREDENTIALS", "Email or password is incorrect.");
            }
            const AuthenticatedUser user{
                rows[0]["user_id"].as<std::int64_t>(),
                rows[0]["name"].c_str(),
                rows[0]["email"].c_str(),
                rows[0]["phone"].c_str(),
                rows[0]["role"].c_str()};
            crow::json::wvalue response;
            response["user"] = userJson(user);
            response["token"] = createSession(database, user.user_id);
            return crow::response(response);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Login failed: " << exception.what() << std::endl;
            return api::errorResponse(503, "DATABASE_UNAVAILABLE", "Login is temporarily unavailable.");
        } });

    CROW_ROUTE(app, "/api/auth/logout").methods(crow::HTTPMethod::POST)
    ([&database](const crow::request &request)
     {
        try
        {
            const std::string authorization = request.get_header_value("Authorization");
            if (authorization.rfind("Bearer ", 0) == 0)
            {
                database.executeWrite(
                    "DELETE FROM auth_sessions WHERE token_hash = $1",
                    pqxx::params{auth::hashToken(authorization.substr(7))});
            }
            return crow::response(204);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Logout failed: " << exception.what() << std::endl;
            return api::errorResponse(503, "DATABASE_UNAVAILABLE", "Logout is temporarily unavailable.");
        } });

    CROW_ROUTE(app, "/api/auth/me")
    ([&database](const crow::request &request)
     {
        try
        {
            const auto user = findAuthenticatedUser(database, request);
            if (!user)
            {
                return unauthorized();
            }
            crow::json::wvalue response;
            response["user"] = userJson(*user);
            return crow::response(response);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Session lookup failed: " << exception.what() << std::endl;
            return api::errorResponse(503, "DATABASE_UNAVAILABLE", "The session could not be checked.");
        } });

    CROW_ROUTE(app, "/api/profile").methods(crow::HTTPMethod::GET)
    ([&database](const crow::request &request)
     {
        try
        {
            const auto user = findAuthenticatedUser(database, request);
            if (!user)
            {
                return unauthorized();
            }
            crow::json::wvalue response;
            response["user"] = userJson(*user);
            return crow::response(response);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Profile lookup failed: " << exception.what() << std::endl;
            return api::errorResponse(503, "DATABASE_UNAVAILABLE", "The profile could not be loaded.");
        } });

    CROW_ROUTE(app, "/api/profile").methods(crow::HTTPMethod::PUT)
    ([&database](const crow::request &request)
     {
        try
        {
            const auto user = findAuthenticatedUser(database, request);
            if (!user)
            {
                return unauthorized();
            }
            const auto body = crow::json::load(request.body);
            const std::string name = jsonString(body, "name");
            const std::string phone = jsonString(body, "phone");
            if (!validation::requiredString(name, "name").valid || name.size() > 100)
            {
                return api::errorResponse(400, "INVALID_NAME", "Name is required and must be at most 100 characters.");
            }
            const pqxx::result rows = database.executeWrite(
                "UPDATE users SET name = $1, phone = NULLIF($2, '') "
                "WHERE user_id = $3 "
                "RETURNING user_id, name, email, COALESCE(phone, '') AS phone, role",
                pqxx::params{name, phone, user->user_id});
            const AuthenticatedUser updated{
                rows[0]["user_id"].as<std::int64_t>(),
                rows[0]["name"].c_str(),
                rows[0]["email"].c_str(),
                rows[0]["phone"].c_str(),
                rows[0]["role"].c_str()};
            crow::json::wvalue response;
            response["user"] = userJson(updated);
            return crow::response(response);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Profile update failed: " << exception.what() << std::endl;
            return api::errorResponse(503, "DATABASE_UNAVAILABLE", "The profile could not be updated.");
        } });

    CROW_ROUTE(app, "/api/profile/password").methods(crow::HTTPMethod::PUT)
    ([&database](const crow::request &request)
     {
        try
        {
            const auto user = findAuthenticatedUser(database, request);
            if (!user)
            {
                return unauthorized();
            }
            const auto body = crow::json::load(request.body);
            const std::string current_password = jsonString(body, "current_password");
            const std::string new_password = jsonString(body, "new_password");
            const pqxx::result rows = database.executeRead(
                "SELECT password_hash FROM users WHERE user_id = $1",
                pqxx::params{user->user_id});
            if (rows.empty() ||
                !auth::verifyPassword(current_password, rows[0]["password_hash"].c_str()))
            {
                return api::errorResponse(401, "INVALID_CURRENT_PASSWORD", "The current password is incorrect.");
            }
            if (new_password.size() < 8)
            {
                return api::errorResponse(400, "INVALID_PASSWORD", "New password must be at least 8 characters.");
            }
            database.executeWrite(
                "UPDATE users SET password_hash = $1 WHERE user_id = $2",
                pqxx::params{auth::hashPassword(new_password), user->user_id});
            return crow::response(204);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Password update failed: " << exception.what() << std::endl;
            return api::errorResponse(503, "DATABASE_UNAVAILABLE", "The password could not be updated.");
        } });

    CROW_ROUTE(app, "/api/parking-areas")
    ([&database](const crow::request &)
     {
        try
        {
            const pqxx::result rows = database.executeRead(
                "SELECT pa.area_id, pa.name, pa.location, "
                "pa.operating_hours, pa.capacity, pa.status, "
                "COUNT(ps.slot_id) FILTER (WHERE ps.status = 'available') AS available_slots "
                "FROM parking_areas pa "
                "LEFT JOIN parking_slots ps ON ps.area_id = pa.area_id "
                "GROUP BY pa.area_id "
                "ORDER BY pa.area_id");

            crow::json::wvalue response;
            response["areas"] = crow::json::wvalue::list();
            std::size_t area_index = 0;
            for (const auto &row : rows)
            {
                response["areas"][area_index++] = api::areaJson(
                    row["area_id"].as<std::int64_t>(),
                    row["name"].c_str(),
                    row["location"].c_str(),
                    row["operating_hours"].is_null() ? "" : row["operating_hours"].c_str(),
                    row["capacity"].as<int>(),
                    row["status"].c_str(),
                    row["available_slots"].as<std::int64_t>());
            }
            return crow::response(response);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Parking-area query failed: " << exception.what() << std::endl;
            return api::errorResponse(
                503,
                "DATABASE_UNAVAILABLE",
                "Parking areas could not be loaded. Check the backend logs.");
        } });

    CROW_ROUTE(app, "/api/parking-areas/<uint>/slots")
    ([&database](unsigned int area_id)
     {
        try
        {
            const pqxx::result area_rows = database.executeRead(
                "SELECT area_id, name FROM parking_areas WHERE area_id = $1",
                pqxx::params{area_id});
            if (area_rows.empty())
            {
                return api::errorResponse(
                    404,
                    "PARKING_AREA_NOT_FOUND",
                    "The requested parking area does not exist.");
            }

            const pqxx::result slot_rows = database.executeRead(
                "SELECT slot_id, area_id, slot_number, vehicle_type, "
                "slot_type, is_ev, status "
                "FROM parking_slots WHERE area_id = $1 ORDER BY slot_number",
                pqxx::params{area_id});

            crow::json::wvalue response;
            response["area"]["area_id"] = area_rows[0]["area_id"].as<std::int64_t>();
            response["area"]["name"] = area_rows[0]["name"].c_str();
            response["slots"] = crow::json::wvalue::list();
            std::size_t slot_index = 0;
            for (const auto &row : slot_rows)
            {
                response["slots"][slot_index++] = api::slotJson(
                    row["slot_id"].as<std::int64_t>(),
                    row["area_id"].as<std::int64_t>(),
                    row["slot_number"].c_str(),
                    row["vehicle_type"].c_str(),
                    row["slot_type"].c_str(),
                    row["is_ev"].as<bool>(),
                    row["status"].c_str());
            }
            return crow::response(response);
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Parking-slot query failed: " << exception.what() << std::endl;
            return api::errorResponse(
                503,
                "DATABASE_UNAVAILABLE",
                "Parking slots could not be loaded. Check the backend logs.");
        } });

    std::cout << "Server running on http://localhost:" << config.server_port << std::endl;

    app.port(config.server_port).multithreaded().run();

    return 0;
}
