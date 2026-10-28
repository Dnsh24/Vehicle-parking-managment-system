#include "application/application.h"

#include <cstdlib>
#include <exception>
#include <iostream>

#ifdef DELETE
#undef DELETE
#endif

namespace
{
void configureDatabasePasswordEnvironment()
{
    const char *password = std::getenv("PARKING_DB_PASSWORD");
    if (password != nullptr && password[0] != '\0')
    {
        return;
    }

    const char *postgres_password = std::getenv("PGPASSWORD");
    if (postgres_password == nullptr || postgres_password[0] == '\0')
    {
        return;
    }

#ifdef _WIN32
    _putenv_s("PARKING_DB_PASSWORD", postgres_password);
#else
    setenv("PARKING_DB_PASSWORD", postgres_password, 1);
#endif
}
} // namespace

int main()
{
    configureDatabasePasswordEnvironment();

    try
    {
        const BackendConfig config = BackendConfig::fromEnvironment();
        ParkingApplication application(config);
        application.run();
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Backend startup error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
