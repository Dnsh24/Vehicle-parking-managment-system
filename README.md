# Vehicle Parking Management System

A college project for managing vehicle parking through an HTML/CSS/JavaScript
frontend, a C++ backend using Crow, and a PostgreSQL database.

## Current development status

| Component | Status |
| --- | --- |
| Frontend | Basic test page implemented. It calls the two backend routes described in [API.md](API.md). |
| PostgreSQL | Schema implemented with 12 core tables, constraints, and indexes. See [database.md](database.md). |
| C++ backend | Initial Crow server implemented across `backend/src/`, with centralized configuration and a database helper. It checks PostgreSQL connectivity and exposes two test routes. |
| REST API | Two test endpoints implemented: `/api/hello` and `/api/database`. Parking-management endpoints are pending. |
| Frontend-backend integration | Basic integration implemented through browser `fetch` calls to `http://localhost:3000`. |

The database schema does not by itself mean that the corresponding user-facing
features or CRUD operations are implemented.

## Technology stack

- HTML, CSS, and JavaScript frontend
- C++20 backend
- Crow, fetched by CMake
- PostgreSQL
- libpqxx for C++ to PostgreSQL access
- CMake and Ninja-compatible build configuration
- JSON responses from the Crow routes

## Planned architecture

The intended architecture is:

```text
HTML/CSS/JavaScript frontend
            |
        HTTP/JSON
            v
       C++ Crow backend
            |
          libpqxx
            v
       PostgreSQL database
```

Only the basic connectivity path and test routes are currently implemented.
Authentication, parking reservations, payments, QR processing, and the
remaining application APIs are planned rather than complete.

## Implemented features

### Frontend

- Displays a parking-management test page.
- Provides a button for testing `/api/hello`.
- Provides a button for testing `/api/database`.
- Displays loading, success, and connection-error messages.
- Uses CORS-compatible requests to the backend at port 3000.

### Backend

- Creates a Crow application with CORS middleware.
- Attempts a PostgreSQL connection to the `parking_management` database.
- Executes `SELECT 1` when the connection is established.
- Exposes `GET /api/hello`.
- Exposes `GET /api/database`.
- Returns a database connection status in the database test response.

### Database

- Defines the 12 core tables documented in [database.md](database.md).
- Uses `pgcrypto` and `gen_random_uuid()` for QR token generation.
- Defines primary keys, foreign keys, checks, unique constraints, and indexes.

## Planned features

The following are described in [planning.md](planning.md) but are not yet
implemented as complete application functionality:

- User registration, authentication, and profiles
- Vehicle registration and management
- Parking-area and parking-slot management
- Availability search and slot allocation
- Reservations and reservation expiry
- QR-based check-in and vehicle entry
- Parking sessions, exit processing, and overstay calculation
- Fee calculation, payments, and digital receipts
- Notifications, history, administration, and reporting

## Actual project structure

```text
.
├── README.md
├── planning.md
├── API.md
├── OOPS.md
├── database.md
├── backend
│   ├── CMakeLists.txt
│   └── src
│       ├── api
│       │   └── errors.h
│       ├── config
│       │   ├── config.cpp
│       │   └── config.h
│       ├── database
│       │   ├── database.cpp
│       │   └── database.h
│       ├── main.cpp
│       └── validation
│           ├── validation.cpp
│           └── validation.h
│   └── tests
│       ├── api_tests.cpp
├── config_tests.cpp
├── database_tests.cpp
│       └── validation_tests.cpp
├── database
│   └── schema.sql
└── frontend
    ├── index.html
    ├── css
    │   └── style.css
    └── js
        └── app.js
```

The repository also contains generated files under `backend/build/`. They are
build artifacts rather than source files and are omitted from the tree above.

## Setup and running

### Frontend

Open `frontend/index.html` in a browser or serve the `frontend` directory with
a local static HTTP server. The page expects the backend at
`http://localhost:3000`.

### Database

Create a PostgreSQL database named `parking_management`, then run:

```text
psql -U <user> -d parking_management -f database/schema.sql
```

The schema file creates the `pgcrypto` extension and all 12 tables. Do not put
real passwords in documentation or source code.

### Backend

The backend requires CMake 3.20 or newer, a C++20 compiler, Crow, and
libpqxx. Configure and build from the `backend` directory using the existing
CMake configuration:

```text
cmake -S backend -B backend/build
cmake --build backend/build
```

Before starting the backend, configure the database password through the
environment. The other variables have local-development defaults:

```text
PARKING_DB_HOST=localhost
PARKING_DB_PORT=5432
PARKING_DB_NAME=parking_management
PARKING_DB_USER=postgres
PARKING_DB_PASSWORD=<your-local-password>
PARKING_SERVER_PORT=3000
```

On Windows PowerShell, for the current terminal session:

```powershell
$env:PARKING_DB_PASSWORD = "<your-local-password>"
```

Run the generated `parking_backend` executable. It listens on the configured
server port. The backend exits with a configuration error if
`PARKING_DB_PASSWORD` is missing or `PARKING_SERVER_PORT` is invalid.

## Documentation

- [planning.md](planning.md) — requirements, progress, and next steps
- [database.md](database.md) — exact PostgreSQL schema documentation
- [API.md](API.md) — implemented API routes and pending API work
- [OOPS.md](OOPS.md) — C++ object-oriented programming analysis

## Limitations and next steps

The project currently demonstrates the initial frontend/backend/database
connection only. The next implementation step is to replace the test routes
with validated application services and REST resources, then connect those
services to the existing schema. Authentication, authorization, input
validation, secrets management, and production deployment also remain
necessary before the system can be considered complete.
