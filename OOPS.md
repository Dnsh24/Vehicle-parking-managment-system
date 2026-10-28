# C++ OOP in the Vehicle Parking Backend

## Scope

This document describes custom C++ code in `backend/src/`. It distinguishes
implemented OOP concepts from concepts the project does not use. Types supplied
by Crow, libpqxx, OpenSSL, and the C++ standard library are dependencies, not
custom project classes.

The backend now uses classes to own application startup and route registration,
with `Database` providing the data-access and transactional workflow layer.
Route groups are represented by controller classes that receive the shared
database dependency. Small structs carry configuration, request identity, fee
values, and typed workflow results. Database records such as users, vehicles,
reservations, and parking sessions remain SQL rows and JSON; they are not yet
custom domain entity classes.

## OOP concepts implemented

| Concept | Implementation | Evidence |
| --- | --- | --- |
| Classes and objects | `ParkingApplication`, `Database`, and five route controller classes; `main()` creates the application object. | `backend/src/application/application.h`, `backend/src/database/database.h`, `backend/src/routes/*_routes.h`, `backend/src/main.cpp` |
| Encapsulation | `ParkingApplication` keeps configuration, database, Crow app, and controllers private. Each controller keeps its database reference private. `Database` keeps connection settings private. | `backend/src/application/application.h`, `backend/src/routes/*_routes.h`, `backend/src/database/database.h` |
| Abstraction | `ParkingApplication::run()` hides CORS setup, database health checking, route wiring, and server startup. Route controller methods hide endpoint registration details. `Database` hides connection creation and transaction details. | `backend/src/application/application.cpp`, `backend/src/database/database.cpp` |
| Composition / has-a | `ParkingApplication` owns its `BackendConfig`, `Database`, Crow app, and route controller objects as members. `Database` owns its configuration by value. | `backend/src/application/application.h`, `backend/src/database/database.h` |
| Association / dependency injection | Each route controller receives a reference to the shared `Database` in its constructor. The application creates the database before its controllers, ensuring the referenced object outlives them. | `backend/src/application/application.cpp`, `backend/src/routes/*_routes.h` |
| Access control | Application state and controller database references are private; public methods expose run and route-registration operations. | `backend/src/application/application.h`, `backend/src/routes/*_routes.h` |
| Constructors | `ParkingApplication`, `Database`, and route controllers initialize from their dependencies. The `Database` constructor is explicit. | `backend/src/application/application.cpp`, `backend/src/database/database.h`, `backend/src/routes/` |
| Static member function | `BackendConfig::fromEnvironment()` constructs configuration without needing an existing object. | `backend/src/config/config.h`, `backend/src/config/config.cpp` |
| Value types and enums | Structs represent validation results, authenticated users, fee summaries, and operation outcomes. `enum class` types represent workflow status values. | `backend/src/validation/validation.h`, `backend/src/routes/route_helpers.h`, `backend/src/database/database.h`, `backend/src/api/session_responses.h` |
| Namespaces and modularity | `routes`, `api`, `auth`, `validation`, `parking::fees`, and `parking::reservations` group related types and operations. | `backend/src/` |
| RAII and object lifetime | Application members have automatic lifetimes. Local libpqxx connections and transactions release or roll back resources during scope exit. | `backend/src/application/application.h`, `backend/src/database/database.cpp` |
| Exception handling | Startup and route/database operations catch standard and libpqxx exceptions and convert failures into diagnostics or HTTP responses. | `backend/src/main.cpp`, `backend/src/routes/`, `backend/src/config/config.cpp` |

## Related C++ concepts

- **Function templates:** reservation response serialization uses templates for
  row-like input types. This is compile-time generic programming, not runtime
  polymorphism. See `backend/src/routes/reservation_routes.cpp`.
- **Standard library types:** `std::string`, `std::optional`, `std::vector`,
  fixed-width integers, and `std::initializer_list` represent text, optional
  values, collections, identifiers, and validation inputs.
- **Free functions:** some validation, authentication, response serialization,
  and route helper operations remain free functions inside namespaces.

## Concepts not implemented in custom code

| Concept | Current status |
| --- | --- |
| Inheritance | No custom class derives from another project class or forms a custom base-class hierarchy. |
| Runtime polymorphism | No custom virtual functions, abstract interfaces, or overridden methods. Crow's framework types are external. |
| Custom operator overloading | No custom `operator` overloads. |
| Custom destructors | No project class declares a destructor; compiler-generated destruction and RAII are used. |
| Smart-pointer ownership | The application uses direct member ownership and references; no custom `std::unique_ptr` or `std::shared_ptr` ownership model is present. |
| Domain entity classes | `User`, `Vehicle`, `Reservation`, and `ParkingSession` are represented as SQL rows and JSON, not C++ classes. |

## Design summary

The project demonstrates encapsulation, abstraction, composition, dependency
injection, and RAII through its application, controller, and database classes.
It does not force inheritance or runtime polymorphism where the current
requirements do not need them. If the backend later needs swappable data stores
or behavior, interfaces can be introduced with a concrete use case and tests.
