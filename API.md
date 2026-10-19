# REST API

## Current status

The API now includes the first read-only parking-availability resources.
Authentication and parking-management write operations are not implemented.

| Component | Current status |
| --- | --- |
| Crow backend | Implemented as an initial server |
| REST endpoints | Health endpoints and parking-availability read endpoints implemented |
| PostgreSQL through C++ | Connectivity check implemented |
| Frontend API integration | Implemented for the two test endpoints |
| Authentication and authorization | Not implemented |
| Parking resources | Not implemented |

The database tables are not API resources until backend routes and operations
are implemented for them.

## Implemented endpoints

### `GET /api/hello`

Returns a JSON message confirming that the C++ backend route is running.

Example response:

```json
{
  "message": "C++ backend is working"
}
```

### `GET /api/database`

Returns the database connection result recorded when the server started. The
backend attempts to connect to PostgreSQL and executes `SELECT 1` before
starting the server.

Successful response:

```json
{
  "status": "success",
  "message": "C++ backend connected to PostgreSQL",
  "database": "parking_management"
}
```

When the startup database check failed, the endpoint returns HTTP `503
Service Unavailable` with the standard API error shape:

```json
{
  "error": {
    "code": "DATABASE_UNAVAILABLE",
    "message": "PostgreSQL is unavailable. Check the backend logs."
  }
}
```

The route reports the startup connectivity result; it does not perform a new
database query for each request. Other future API errors should use the same
`error.code` and `error.message` structure.

### `GET /api/parking-areas`

Returns every configured parking area. Under the Phase 1 design, each mall
floor is represented as a separate parking area. `available_slots` is
calculated from the current slot statuses.

Example response:

```json
{
  "areas": [
    {
      "area_id": 1,
      "name": "Mall Central — Ground Floor",
      "location": "Mall Central",
      "operating_hours": "07:00 - 22:00",
      "capacity": 32,
      "status": "active",
      "available_slots": 12
    }
  ]
}
```

Database failures return `503 DATABASE_UNAVAILABLE`.

### `GET /api/parking-areas/{area_id}/slots`

Returns the selected floor and all of its configured parking slots.

Example response:

```json
{
  "area": {
    "area_id": 1,
    "name": "Mall Central — Ground Floor"
  },
  "slots": [
    {
      "slot_id": 1,
      "area_id": 1,
      "slot_number": "G-A01",
      "vehicle_type": "car",
      "slot_type": "standard",
      "is_ev": false,
      "status": "available"
    }
  ]
}
```

An unknown area returns `404 PARKING_AREA_NOT_FOUND`. Database failures
return `503 DATABASE_UNAVAILABLE`.

## Frontend usage

`frontend/js/app.js` calls both routes using `fetch` against
`http://localhost:3000`. The page displays the returned message and reports a
connection error when the backend cannot be reached.

## Planned API work

The following areas are planned, not implemented:

- Authentication and user management
- Vehicle management
- Parking-area, slot, and rate management writes
- Availability and reservation operations
- QR-token check-in
- Parking sessions and checkout
- Payments, receipts, and notifications

Request and response contracts should be added here only when their routes are
implemented and tested.
