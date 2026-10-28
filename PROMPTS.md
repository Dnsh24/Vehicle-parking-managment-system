# Project Prompts by Phase

This is a phase-based prompt log for the Vehicle Parking Management System.
Prompt counts vary with each phase's scope. Phases 0 to 8 describe implemented
Version 1 work; Phases 9 to 12 are planned prompts and do not claim implementation.
Paths refer to the current repository. The PostgreSQL schema source is
`database/schema.sql`; migrations and development seeds are in `database/`.

## Phase 0: Foundation (3 prompts)

### Prompt 1: Repository and architecture
> Set up the HTML/CSS/JavaScript frontend, C++20 Crow backend, PostgreSQL database accessed through libpqxx, and CMake build. Document the actual project structure and request flow in README.md.

### Prompt 2: Backend configuration and database access
> Add environment-based backend configuration and a reusable libpqxx database helper. Keep credentials out of source control, use parameterized queries and transaction objects, and expose health and database connectivity routes.

### Prompt 3: Validation and backend tests
> Add shared request validation and JSON error helpers, then cover configuration, database behavior, and response shapes with focused backend tests.

## Phase 1: Parking availability (4 prompts)

### Prompt 1: Schema and local data
> Define parking areas, slots, their constraints, and useful indexes in `database/schema.sql`. Add repeatable local seed data for three floors with mixed slot types and states.

### Prompt 2: Availability API
> Add read-only API routes for parking areas and their slots. Return consistent JSON and useful errors for invalid areas, empty results, and database failures.

### Prompt 3: Availability interface
> Build a responsive floor selector and slot grid with status colors, summary counts, a legend, vehicle icons, loading/empty/error states, and retry behavior.

### Prompt 4: Frontend integration
> Connect the availability interface to the Crow API with `fetch`, then document the implemented routes and local setup.

## Phase 2: Authentication and profiles (3 prompts)

### Prompt 1: Authentication storage and security
> Add username and bearer-session support to the schema and migration. Implement password hashing, secure token creation, and token hashing so raw bearer tokens are never stored.

### Prompt 2: Auth and profile API
> Implement registration, login, logout, current-user, profile update, and password update routes. Enforce authentication, account status, duplicate-identifier checks, validation, and consistent errors.

### Prompt 3: Protected frontend
> Add login and registration views, session restoration, bearer headers, logout, and protected application views. Document request and response behavior in `API.md`.

## Phase 3: Vehicle management (3 prompts)

### Prompt 1: Vehicle and assignment storage
> Add owner-linked vehicle fields and active parking assignments with constraints that prevent a vehicle or slot from having multiple active assignments. Provide a safe migration for existing data.

### Prompt 2: Owner-scoped vehicle operations
> Implement authenticated vehicle CRUD, registration normalization, vehicle-type and EV validation, compatible-slot lookup, atomic parking, and checkout.

### Prompt 3: Vehicle interface
> Add vehicle management, compatible slot selection, parking state, and checkout to the frontend. Refresh availability after state changes and document the API.

## Phase 4: Administration and maintenance (3 prompts)

### Prompt 1: Authorization and admin API
> Add role-protected operations for user access, parking areas, slots, rates, and maintenance. Prevent unsafe deletion and reject duplicate or conflicting data.

### Prompt 2: Admin console
> Add an administrator view with navigation and management forms for users, areas, slots, rates, and maintenance. Validate input and show operation results.

### Prompt 3: Admin development data and verification
> Add repeatable development seed data for an admin, baseline rates, and maintenance records. Verify role boundaries and CRUD behavior, then update `API.md`, `planning.md`, and `README.md`.

## Phase 5: Search and reservations (4 prompts)

### Prompt 1: Reservation availability
> Add filtered availability search by area, vehicle, slot type, EV/accessibility needs, and requested time. Exclude slots with conflicts or maintenance.

### Prompt 2: Transactional booking
> Implement owner-scoped reservation create, list, detail, and cancel operations. Lock the selected slot and validate vehicle ownership, compatibility, time ranges, and overlap within transactions.

### Prompt 3: Reservation interface
> Add search filters, reservation creation, confirmation, status display, and cancellation to the frontend.

### Prompt 4: Integration and API documentation
> Verify normal and error flows, including conflicts, invalid ranges, and cross-user access. Update the API reference and phase plan to match tested behavior.

## Phase 6: Manual entry and parking sessions (2 prompts)

### Prompt 1: Manual check-in
> Implement authenticated reservation check-in with time-window and ownership checks. In one transaction, create a parking session, activate the reservation, and occupy the slot.

### Prompt 2: Active session experience
> Add reservation check-in controls and active session views. Cover duplicate, expired, too-early, unavailable-slot, and unauthorized cases in tests and docs.

## Phase 7: Exit, fees, and settlement status (3 prompts)

### Prompt 1: Fee preview and calculation
> Implement session fee summaries using parking duration, hourly rates, reserved end time, and overstay rates. Add response types and unit coverage.

### Prompt 2: Transactional exit
> Add owner-scoped session exit. Atomically complete the session and reservation, record payment status, calculate fees, and release the slot unless maintenance applies.

### Prompt 3: Checkout experience
> Add fee preview and checkout controls to the active-session interface, including manual paid, unpaid, and waived status. Verify duplicate exit, ownership isolation, and slot release.

## Phase 8: History and in-app notifications (3 prompts)

### Prompt 1: History and notification data access
> Add user-scoped, paginated history and notification operations, unread counts, and indexes for common list queries.

### Prompt 2: Workflow events
> Write reservation, check-in, exit, overstay, and settlement notifications in the same transactions as their associated state changes.

### Prompt 3: Frontend and verification
> Add history and notification views, unread indicators, and mark-as-read controls. Verify pagination, empty results, and user isolation; update project docs.

## Phase 9: QR check-in and automated expiry (planned, 2 prompts)

### Prompt 1: Expiry and QR lifecycle
> Add reservation-linked QR token creation, expiry, single-use validation, and automated expiration of reservations that miss the check-in window.

### Prompt 2: Entry interface and safeguards
> Add QR display/scanning and entry results. Test invalid, expired, reused, and unauthorized tokens and ensure duplicate entry is rejected.

## Phase 10: Online payments and digital receipts (planned, 2 prompts)

### Prompt 1: Payment processing
> Add payment initiation and provider verification with amount validation, idempotency, failure handling, and refund transitions.

### Prompt 2: Receipts
> Generate owner-scoped digital receipts after verified payment, and add receipt details and optional download to the frontend.

## Phase 11: Advanced fees and notifications (planned, 2 prompts)

### Prompt 1: Fee policies
> Extend rate rules for vehicle, area, slot, and EV conditions, with explicit rounding, minimum charges, and advanced overstay policies.

### Prompt 2: Maintenance and delivery
> Add scheduled maintenance workflows and expanded notification events. Add email or SMS delivery only after in-app event behavior is reliable.

## Phase 12: Dashboard and reports (planned, 2 prompts)

### Prompt 1: Reporting API
> Add administrator-only aggregates for users, vehicles, capacity, reservations, payments, revenue, and overstays, with date filters and bounded queries.

### Prompt 2: Dashboard interface and verification
> Add charts, filters, tables, and optional exports. Verify totals against known records, date boundaries, empty periods, and role restrictions.
