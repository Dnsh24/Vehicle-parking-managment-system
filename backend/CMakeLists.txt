cmake_minimum_required(VERSION 3.20)

project(parking_backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

include(FetchContent)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Crow)

find_package(libpqxx CONFIG REQUIRED)

add_executable(parking_backend
    src/main.cpp
)

target_link_libraries(parking_backend
    PRIVATE
    Crow::Crow
    libpqxx::pqxx
)