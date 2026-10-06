# Parking Management System

## 1. Introduction

The **Parking Management System** is a digital platform designed to simplify and automate parking management for users, parking operators, and administrators.

The system allows users to register their vehicles, search for available parking spaces, make reservations, check in using QR codes, manage parking sessions, make payments, and view their parking history.

Administrators can manage users, parking areas, parking slots, reservations, maintenance, payments, and system reports through an administrative dashboard.

---

# 2. User Features

## 2.1 User Registration and Login

Users can create an account and securely log into the system.

### Functions

- User registration
- User login/logout
- Account authentication
- Password management
- User account validation

---

## 2.2 User Profile

Users can manage their personal information from their profile.

### Functions

- View profile information
- Update personal details
- Manage account information
- View registered vehicles

---

## 2.3 Vehicle Management

Users can register and manage their vehicles.

### Functions

- Add a vehicle
- Update vehicle information
- Delete a vehicle
- View registered vehicles
- Set vehicle type

### Example Vehicle Types

- Car
- Bike
- SUV
- Electric Vehicle (EV)
- Other supported vehicle categories

Vehicle information can be associated with parking reservations and QR-based entry/exit.

---

# 3. Parking Management

## 3.1 Multiple Parking Areas

The system supports multiple independent parking areas.

Each parking area can have its own:

- Name
- Location
- Operating hours
- Parking capacity
- Available slots
- Vehicle-specific slots
- EV charging facilities

This allows the system to manage parking facilities across different locations.

---

## 3.2 Vehicle-Specific Parking Slots

Parking slots can be assigned to specific vehicle types.

For example:

| Slot Type | Allowed Vehicle   |
| --------- | ----------------- |
| Car Slot  | Cars              |
| Bike Slot | Bikes             |
| SUV Slot  | SUVs              |
| EV Slot   | Electric Vehicles |

The system prevents users from reserving a slot that is incompatible with their vehicle.

---

## 3.3 Parking Slot Management

The system maintains the current state of every parking slot.

### Possible Slot States

- Available
- Reserved
- Occupied
- Maintenance
- Unavailable

The slot status is updated whenever a reservation, entry, exit, or maintenance operation occurs.

---

## 3.4 EV/Charging Slots

Dedicated parking spaces can be configured for electric vehicles.

### Functions

- Identify EV-compatible slots
- Display charging availability
- Reserve EV parking slots
- Track charging-enabled spaces
- Prevent non-EV vehicles from reserving EV-only slots

---

## 3.5 Parking Availability

Users can view real-time parking availability.

The system can display:

- Total slots
- Available slots
- Reserved slots
- Occupied slots
- Maintenance slots
- EV slots

Availability should be updated whenever the status of a parking slot changes.

---

## 3.6 Search and Filter

Users can search for suitable parking spaces based on different criteria.

### Possible Filters

- Parking area
- Vehicle type
- Slot type
- EV availability
- Availability
- Reservation time
- Distance/location

This helps users quickly find an appropriate parking space.

---

# 4. Reservation System

## 4.1 Parking Reservation

Users can reserve an available parking slot for a specific period.

### Reservation Information

- User
- Vehicle
- Parking area
- Parking slot
- Reservation date
- Start time
- End time
- Reservation status
- Reservation ID

### Reservation States

- Pending
- Confirmed
- Active
- Completed
- Expired
- Cancelled

---

## 4.2 Reservation Expiry

Reservations are automatically monitored for expiry.

If a user does not check in within the allowed time period, the reservation can be marked as **Expired**.

The associated parking slot can then be released and made available for other users.

---

# 5. QR-Based Check-In

Users can use a QR code to check into the parking facility.

### Check-In Flow

1. User creates a reservation.
2. System generates a unique reservation/QR identifier.
3. User arrives at the parking area.
4. User scans the QR code.
5. Backend identifies the reservation.
6. User authentication is verified.
7. Reservation status is checked.
8. Vehicle and slot information are validated.
9. Reservation is activated.
10. Vehicle entry is recorded.

The QR code should identify the **reservation**, rather than relying on the phone that performs the scan.

This allows a user to scan the QR code using a different mobile device while the backend still identifies the correct reservation.

---

# 6. Vehicle Entry

The system records vehicle entry into the parking facility.

### Entry Validation

Before allowing entry, the system verifies:

- QR code validity
- Reservation ID
- Reservation status
- User authentication
- Vehicle information
- Parking area
- Assigned parking slot
- Reservation time

If all conditions are satisfied, the reservation becomes active and the vehicle entry time is recorded.

---

# 7. Vehicle Exit

When the vehicle leaves the parking facility, the system records the exit.

### Exit Process

1. User initiates exit.
2. Reservation/vehicle session is identified.
3. Entry time is retrieved.
4. Exit time is recorded.
5. Parking duration is calculated.
6. Any additional charges are calculated.
7. Payment status is verified.
8. Reservation is completed.
9. Parking slot becomes available again.
10. Digital receipt is generated.

---

# 8. Overstay Detection

The system automatically detects vehicles that remain parked beyond their reserved period.

### Example

If a user reserves a slot from:

**10:00 AM → 12:00 PM**

but exits at:

**1:00 PM**

the system identifies the additional one-hour period as an **overstay**.

The system can then:

- Calculate additional fees
- Notify the user
- Record the overstay
- Update the reservation status

---

# 9. Fee Calculation

The system automatically calculates parking fees.

The fee can depend on:

- Parking duration
- Vehicle type
- Parking area
- Slot type
- EV charging usage
- Reservation duration
- Overstay duration

### Example

**Base Parking Fee**

`Parking Duration × Hourly Rate`

Additional charges such as overstay fees can then be added to the base amount.

---

# 10. Payment

Users can pay their parking charges through the system.

### Functions

- Display amount due
- Initiate payment
- Verify payment
- Store payment status
- Associate payment with reservation
- Generate payment record

### Payment States

- Pending
- Successful
- Failed
- Refunded

---

# 11. Digital Receipt

After a successful payment, the system generates a digital receipt.

### Receipt Information

- Receipt ID
- Reservation ID
- User
- Vehicle
- Parking area
- Parking slot
- Entry time
- Exit time
- Parking duration
- Base fee
- Overstay fee
- Additional charges
- Total amount
- Payment status
- Payment date

---

# 12. Parking History

Users can view their previous parking sessions.

### History Information

- Parking area
- Vehicle
- Parking slot
- Reservation date
- Entry time
- Exit time
- Duration
- Amount paid
- Payment status
- Reservation status

Users can use this section to review their previous parking activity.

---

# 13. Notifications

The system provides notifications to users regarding important parking events.

### Possible Notifications

- Reservation confirmation
- Upcoming reservation
- Reservation expiry warning
- Reservation expired
- Parking check-in confirmation
- Overstay warning
- Payment confirmation
- Payment failure
- Parking session completion
- Maintenance/unavailability notification

---

# 14. Admin Features

## 14.1 Admin Dashboard

The administrator has access to a centralized dashboard for managing the parking system.

### Dashboard Statistics

- Total users
- Total vehicles
- Total parking areas
- Total parking slots
- Available slots
- Occupied slots
- Reserved slots
- Maintenance slots
- Active reservations
- Completed reservations
- Total payments
- Revenue
- Overstays

---

# 15. User Management

Administrators can manage registered users.

### Functions

- View users
- Search users
- View user details
- View registered vehicles
- Activate/deactivate accounts
- Manage user access

---

# 16. Parking-Area Management

Administrators can create and manage parking facilities.

### Functions

- Add parking area
- Update parking area
- Remove parking area
- Configure parking capacity
- Configure operating hours
- View parking availability
- Manage parking-area status

Each parking area can contain multiple parking slots.

---

# 17. Slot Management

Administrators can manage individual parking slots.

### Functions

- Add slots
- Update slots
- Remove slots
- Assign slot types
- Assign vehicle types
- Configure EV slots
- Change slot status
- View slot occupancy

### Slot Status

Administrators can manually or automatically set slots as:

- Available
- Reserved
- Occupied
- Maintenance
- Unavailable

---

# 18. Maintenance Slots

Parking slots may occasionally need to be unavailable because of maintenance.

Administrators can mark specific slots as **Maintenance**.

### Functions

- Mark slot for maintenance
- Specify maintenance period
- Add maintenance reason
- Prevent reservations during maintenance
- Return slot to available status after maintenance

This prevents users from reserving a slot that cannot currently be used.

---

# 19. Reservation Management

Administrators can monitor and manage all reservations.

### Functions

- View reservations
- Search reservations
- Filter by status
- View reservation details
- Cancel reservations
- Modify reservation status
- Monitor active reservations
- Monitor expired reservations
- Monitor overstays

---

# 20. Payment Management

Administrators can monitor payment transactions.

### Functions

- View transactions
- Search payments
- Filter payments
- View payment status
- View transaction details
- Track successful/failed payments
- Manage refunds where applicable

---

# 21. Reports and Statistics

The system provides reports to help administrators understand parking usage and system performance.

### Possible Reports

#### Parking Usage

- Total parking sessions
- Daily parking sessions
- Weekly parking sessions
- Monthly parking sessions
- Slot utilization

#### Revenue

- Daily revenue
- Weekly revenue
- Monthly revenue
- Revenue by parking area
- Revenue by vehicle type

#### Reservations

- Total reservations
- Completed reservations
- Cancelled reservations
- Expired reservations
- Active reservations

#### Overstay

- Number of overstays
- Total overstay duration
- Overstay revenue

These statistics can help administrators identify busy parking areas, frequently used slots, and revenue trends.

---

# 22. Feature Summary

| Module                     | Features                                           |
| -------------------------- | -------------------------------------------------- |
| **User Account**           | Registration, Login, Profile                       |
| **Vehicle**                | Add, Edit, Delete, Manage Vehicles                 |
| **Parking**                | Multiple Areas, Slots, Vehicle-Specific Slots      |
| **EV Parking**             | EV/Charging Slots                                  |
| **Availability**           | Real-Time Parking Availability                     |
| **Search**                 | Search and Filtering                               |
| **Reservation**            | Create, Manage, Expire, Cancel                     |
| **QR System**              | QR-Based Check-In                                  |
| **Entry**                  | Vehicle Entry Validation                           |
| **Exit**                   | Vehicle Exit Processing                            |
| **Overstay**               | Overstay Detection and Charges                     |
| **Fees**                   | Automatic Fee Calculation                          |
| **Payment**                | Payment Processing                                 |
| **Receipt**                | Digital Receipt                                    |
| **History**                | Parking History                                    |
| **Notifications**          | Reservation, Payment, Overstay Alerts              |
| **Admin**                  | Dashboard and System Management                    |
| **User Management**        | Manage Users                                       |
| **Parking Management**     | Manage Parking Areas                               |
| **Slot Management**        | Manage Parking Slots                               |
| **Maintenance**            | Maintenance Slot Management                        |
| **Reservation Management** | Monitor and Manage Reservations                    |
| **Payment Management**     | Monitor Transactions                               |
| **Reports**                | Parking, Revenue, Reservation and Usage Statistics |

# 23. Overall System Flow

```text
User Registration/Login
        ↓
Add Vehicle
        ↓
Search Parking Areas
        ↓
Check Parking Availability
        ↓
Select Vehicle + Parking Slot
        ↓
Create Reservation
        ↓
Receive Reservation Confirmation + QR
        ↓
Arrive at Parking Area
        ↓
Scan QR Code
        ↓
Validate Reservation
        ↓
Vehicle Entry
        ↓
Parking Session
        ↓
Overstay Detection
        ↓
Vehicle Exit
        ↓
Calculate Parking Fee
        ↓
Payment
        ↓
Digital Receipt
        ↓
Parking History
```

## Admin Flow

```text
Admin Login
     ↓
Admin Dashboard
     ↓
Manage Users
     ↓
Manage Parking Areas
     ↓
Manage Parking Slots
     ↓
Manage EV Slots
     ↓
Manage Maintenance
     ↓
Monitor Reservations
     ↓
Monitor Payments
     ↓
Generate Reports & Statistics
```

# 24. System Objective

The primary objective of the Parking Management System is to provide a **centralized, automated, and efficient parking management solution**.

The system connects users, vehicles, parking areas, parking slots, reservations, entry/exit operations, payments, and administrative management into a single platform.

It aims to reduce manual parking management, improve slot utilization, simplify reservations and payments, and provide administrators with real-time visibility into parking operations.
