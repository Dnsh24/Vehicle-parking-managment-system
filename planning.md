# Vehicle Parking Management System Plan

## 1. Project status

### Completed

- Basic HTML/CSS/JavaScript frontend test page
- Crow C++ server foundation
- Environment-based backend configuration
- PostgreSQL connection check using libpqxx
- Reusable backend validation utilities with focused tests
- Parameterized database read and write operations with transaction handling
- Database foundation failure test
- Configuration and API error-response tests
- Foundation runtime verification, health checks, and CORS verification
- `GET /api/hello`
- `GET /api/database`
- PostgreSQL schema with 12 core tables, constraints, and indexes

### In progress

- Frontend-to-backend integration beyond the health-check page
- Application-specific backend and integration testing

### Planned

- User authentication and profiles
- Vehicle management
- Parking-area and slot management
- Availability search
- Reservations and expiry
- QR check-in and parking sessions
- Entry, exit, overstay, and fee calculation
- Payments, receipts, history, and notifications
- Admin dashboard, management, and reports

The existence of a database table does not mean that its related application
feature is implemented.

## 2. Project objective

Build a centralized parking-management platform for users and administrators.
The intended system connects users, vehicles, parking areas, parking slots,
reservations, entry and exit operations, payments, notifications, and reports.

The technology stack is:

- Frontend: HTML, CSS, JavaScript
- Backend: C++ with Crow
- Database: PostgreSQL
- Communication: REST APIs

## 3. Functional requirements

### User accounts

Users should be able to register, log in, log out, manage profiles, change
passwords, and view their registered vehicles.

**Status:** Planned. The `users` table exists, but authentication is not
implemented.

### Vehicles

Users should be able to add, edit, delete, and view vehicles. Vehicle types
and EV status must be validated. A vehicle must belong to the authenticated
user.

**Status:** Planned. The `vehicles` table exists, but no vehicle API or UI
exists.

### Parking areas and slots

Administrators should manage multiple parking areas, capacities, locations,
operating hours, parking slots, vehicle types, slot types, and EV capability.

Slot states are:

- `available`
- `reserved`
- `occupied`
- `maintenance`
- `unavailable`

**Status:** Planned. The schema supports these values, but management APIs and
screens do not exist.

### Availability and search

Users should search by area, vehicle type, slot type, EV capability,
availability, reservation time, and location.

A slot is selectable only when it:

- Supports the vehicle type
- Meets EV requirements
- Is not unavailable or under maintenance
- Has no conflicting reservation

Real-time availability must not be claimed until backend state updates and
frontend refresh behavior are implemented.

**Status:** Planned.

### Reservations

Users should create, view, and cancel reservations with:

- User
- Vehicle
- Area
- Slot
- Start time
- End time
- Status
- Reservation ID

Reservation statuses are:

- `pending`
- `confirmed`
- `active`
- `completed`
- `expired`
- `cancelled`

Reservation creation must validate ownership, time range, compatibility,
availability, and maintenance state. It must use a transaction to prevent
conflicting bookings.

**Status:** Planned. The `reservations` table exists only.

### Reservation expiry

Reservations that are not used within the defined check-in window should
become expired. A slot may be released only when it is not occupied or under
maintenance.

**Status:** Planned.

### QR check-in and vehicle entry

The QR code must identify the reservation, not the device that scans it.

The check-in flow is:

```text
Reservation → QR token → Scan → Validate token and reservation
→ Validate user, vehicle, time, and slot → Create session
→ Mark reservation active and slot occupied
```

Validation must cover token expiry and reuse, authentication, reservation
status, vehicle/slot compatibility, parking area, and reservation time.

**Status:** Planned. The `qr_tokens` table exists, but no QR or entry logic
exists.

### Parking sessions and exit

On entry, create a parking session and record `entry_time`. On exit:

1. Identify the active session.
2. Record `exit_time`.
3. Calculate duration.
4. Calculate overstay and fees.
5. Complete the session and reservation.
6. Release the slot only when it is not under maintenance.
7. Continue to payment and receipt generation.

The entry and exit state changes must be transactional.

**Status:** Planned. The `parking_sessions` table exists only.

### Overstay and fees

Overstay is calculated from the reserved end time and actual exit time:

```text
overstay_duration = max(0, actual_exit_time - reserved_end_time)
```

Fees may depend on parking duration, vehicle type, area, slot type, EV usage,
reservation duration, and overstay duration.

```text
base_fee = parking_duration × hourly_rate
total_fee = base_fee + overstay_fee + additional_charges
```

Billing units, rounding, minimum charges, and missing-rate behavior must be
defined before implementation.

**Status:** Planned. Rate and fee tables exist, but no calculation service
exists.

### Payments and receipts

Users should be able to initiate and verify payments. Payment statuses are:

- `pending`
- `successful`
- `failed`
- `refunded`

Successful payments should produce receipts containing the reservation,
session, timing, fee breakdown, payment status, and receipt number.

Payment processing must not be described as implemented without a working
payment flow.

**Status:** Planned. The `payments` and `receipts` tables exist only.

### History and notifications

Users should view previous reservations, sessions, durations, payment status,
amounts, and receipts.

Notifications should support reservation confirmation, expiry, check-in,
overstay, payment, session completion, and maintenance events.

**Status:** Planned. The `notifications` table exists, but no services or
frontend screens exist.

## 4. Administrator requirements

Administrators should be able to:

- View dashboard statistics
- Manage users and account access
- Manage parking areas and slots
- Configure EV slots and rates
- Schedule slot maintenance
- Monitor and manage reservations
- Monitor payments and refunds
- Generate parking, revenue, reservation, and overstay reports

Dashboard statistics should include users, vehicles, areas, slots, slot
statuses, reservations, payments, revenue, and overstays.

**Status:** Planned. No admin authentication, APIs, or dashboard exists.

## 5. Build phases

Each phase follows this sequence:

```text
Database review → Backend logic and API → Backend tests
→ Frontend screen → Frontend integration → End-to-end tests
→ Update API.md and this plan
```

Proposed endpoints must not be added to [API.md](API.md) until implemented
and tested.

### Phase 0: Foundation

**Status:** Completed.

- Centralize configuration and database access.
- Add validation utilities and consistent JSON errors.
- Add parameterized-query and transaction conventions.
- Add backend tests.
- Keep the health endpoints working.
- Verify runtime startup, PostgreSQL connectivity, CORS behavior, and secret
  safety.

### Phase 1: Parking availability

**Status:** Next.

- Backend: query areas and slots and expose tested REST endpoints.
- Frontend: display areas, select an area, and show slot states.
- Test invalid areas, empty areas, database failures, occupied slots, and
  maintenance slots.

References: `parking_areas`, `parking_slots`, and `maintenance` in
[database.md](database.md).

### Phase 2: Authentication and profiles

- Backend: registration, login, logout, password hashing, authorization, and
  profile operations.
- Frontend: registration, login, logout, profile, and protected views.
- Test duplicate emails, invalid credentials, inactive accounts, and
  unauthenticated access.

Reference: `users` in [database.md](database.md).

### Phase 3: Vehicle management

- Backend: authenticated vehicle CRUD and ownership validation.
- Frontend: vehicle list, add, edit, delete, vehicle type, and EV controls.
- Test duplicate registrations, invalid types, and unauthorized access.

Reference: `vehicles` in [database.md](database.md).

### Phase 4: Administration and maintenance

- Backend: administrator-only area, slot, rate, user, and maintenance
  operations.
- Frontend: dashboard shell and management screens.
- Test roles, slot compatibility, duplicate slot numbers, maintenance
  periods, and invalid status changes.

References: Sections 4 and 15; related tables in [database.md](database.md).

### Phase 5: Search and reservations

- Backend: filtered availability, reservation creation, listing, details,
  cancellation, and conflict protection.
- Frontend: filters, reservation form, confirmation, status, and cancellation.
- Test ownership, compatibility, time ranges, overlapping reservations, and
  concurrent requests.

References: `reservations`, `parking_slots`, and `parking_rates` in
[database.md](database.md).

### Phase 6: Expiry, QR, entry, and sessions

- Backend: expiry process, reservation-linked QR tokens, check-in validation,
  session creation, and entry state transitions.
- Frontend: expiry warnings, QR display/scanning, check-in result, and active
  session screen.
- Test expired, reused, invalid, and unauthorized tokens, duplicate entry,
  and maintenance slots.

References: `qr_tokens`, `reservations`, and `parking_sessions` in
[database.md](database.md).

### Phase 7: Exit, overstay, fees, payments, and receipts

- Backend: transactional exit, duration, overstay, fee calculation, payment
  processing, and receipt generation.
- Frontend: exit confirmation, fee breakdown, payment screen, and receipt.
- Test normal exit, overstay, duplicate payment, amount tampering, failed
  payment, refund, and receipt ownership.

References: `parking_sessions`, `parking_rates`, `payments`, and `receipts`
in [database.md](database.md).

### Phase 8: History and notifications

- Backend: user-scoped history, receipts, notifications, unread status, and
  event notifications.
- Frontend: history list, receipt links, notification list, unread count, and
  mark-as-read behavior.
- Test user isolation, pagination, empty results, and notification ownership.

References: `reservations`, `parking_sessions`, `payments`, `receipts`, and
`notifications` in [database.md](database.md).

### Phase 9: Admin dashboard and reports

- Backend: authorized dashboard aggregates and reports.
- Frontend: statistics, charts, filters, tables, and optional exports.
- Test totals against known data, date boundaries, empty periods, and roles.

References: Sections 4, 15, and 16.

## 6. Phase dependencies

```text
Foundation
  ↓
Parking availability
  ↓
Authentication
  ↓
Vehicles
  ↓
Administration and maintenance
  ↓
Search and reservations
  ↓
Expiry, QR, entry, and sessions
  ↓
Exit, overstay, fees, payments, and receipts
  ↓
History and notifications
  ↓
Admin dashboard and reports
```

Mark a feature **Completed** only after its backend, frontend, integration,
and relevant error-path tests pass.

## 7. Documentation references

- [README.md](README.md) — overview, setup, and current implementation
- [API.md](API.md) — verified REST endpoints only
- [database.md](database.md) — actual PostgreSQL schema
- [OOPS.md](OOPS.md) — actual C++ structure and OOP concepts
