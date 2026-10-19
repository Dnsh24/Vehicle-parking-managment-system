# C++ OOPS Analysis

## Scope and current status

This document describes object-oriented programming evidence in the project's
custom C++ source. It does not use PostgreSQL tables or third-party Crow and
libpqxx types as evidence of custom project OOPS design.

The current implementation includes a `BackendConfig` data structure and a
`Database` helper in addition to the startup logic in `backend/src/main.cpp`.
It creates a Crow application, configures middleware, checks PostgreSQL
connectivity, and registers two routes. A meaningful application domain class
model has not yet been introduced.

## Concepts

| OOPS concept | Current status | Evidence |
| --- | --- | --- |
| Classes and objects | In progress; one custom database helper exists, but no domain classes | `Database` is a custom class; Crow and libpqxx types are third-party objects |
| Encapsulation | Demonstrated for database-check orchestration | `Database` owns configuration and exposes `checkConnection()` |
| Abstraction | Demonstrated at a basic level | `Database::checkConnection()` hides connection-string construction and query details |
| Inheritance | Not used in custom code | No project inheritance hierarchy |
| Polymorphism | Not used in custom code | No project polymorphic interface |
| Constructors/destructors | Basic constructor present | `Database` receives configuration; no custom destructor is needed |
| Exception handling | Present at a basic level | `main.cpp` catches `std::exception` around the database check |
| STL usage | Basic | `std::string` and standard exception types are used |
| RAII | Demonstrated in the database helper | Local Crow and libpqxx objects manage their own lifetimes |

## Current implementation observations

- `main` owns application setup, route registration, and server startup.
- `BackendConfig::fromEnvironment()` validates environment-based configuration.
- `Database` owns a copy of the connection settings and performs the startup
  connectivity check.
- `Database::executeRead()` runs parameterized read queries in a read
  transaction, while `Database::executeWrite()` commits a write transaction.
- `database_connected` is represented by `DatabaseCheckResult` and captured by
  the database-status route.
- The database connection and transaction are local RAII-managed objects.
- The current source does not define custom domain classes such as `User`,
  `Vehicle`, `Reservation`, or `ParkingSession`.

## Planned OOPS improvements

As implementation grows, classes may be introduced for real responsibilities
such as configuration, database access, validation, and application services.
They should be added only when they improve ownership and testability. Future
changes should document actual constructors, destructors, interfaces,
inheritance, and exception behavior rather than claiming concepts from the
database schema.
