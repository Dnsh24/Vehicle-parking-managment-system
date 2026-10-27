#pragma once

#include <algorithm>
#include <cmath>

namespace parking::fees
{

inline constexpr int free_buffer_minutes = 7;
inline constexpr double overtime_rate_per_minute = 1.0;

struct Breakdown
{
    int billed_hours;
    int billed_overtime_minutes;
    double base_fee;
    double overtime_fee;
    double total_fee;
};

inline double roundCurrency(double amount)
{
    return std::round(amount * 100.0) / 100.0;
}

inline Breakdown calculate(
    int booked_duration_minutes,
    int overtime_duration_seconds,
    double hourly_rate)
{
    const int billed_hours = std::max(1, (booked_duration_minutes + 59) / 60);
    const int nonnegative_overtime_seconds = std::max(0, overtime_duration_seconds);
    const int billed_overtime_minutes =
        nonnegative_overtime_seconds / 60 + (nonnegative_overtime_seconds % 60 != 0 ? 1 : 0);
    const double base_fee = roundCurrency(billed_hours * hourly_rate);
    const double overtime_fee = roundCurrency(billed_overtime_minutes * overtime_rate_per_minute);

    return {
        billed_hours,
        billed_overtime_minutes,
        base_fee,
        overtime_fee,
        roundCurrency(base_fee + overtime_fee)};
}

} // namespace parking::fees
