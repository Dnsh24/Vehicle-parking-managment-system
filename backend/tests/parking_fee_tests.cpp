#include "../src/database/parking_fees.h"

#include <cassert>

int main()
{
    const auto no_overtime = parking::fees::calculate(90, 0, 60.0);
    assert(no_overtime.billed_hours == 2);
    assert(no_overtime.base_fee == 120.0);
    assert(no_overtime.overtime_fee == 0.0);
    assert(no_overtime.total_fee == 120.0);

    const auto one_overtime_second = parking::fees::calculate(60, 1, 60.0);
    assert(one_overtime_second.billed_overtime_minutes == 1);
    assert(one_overtime_second.overtime_fee == 1.0);

    const auto half_overtime_hour = parking::fees::calculate(60, 1800, 60.0);
    assert(half_overtime_hour.base_fee == 60.0);
    assert(half_overtime_hour.overtime_fee == 30.0);
    assert(half_overtime_hour.total_fee == 90.0);

    const auto fifteen_overtime_minutes = parking::fees::calculate(60, 900, 40.0);
    assert(fifteen_overtime_minutes.billed_overtime_minutes == 15);
    assert(fifteen_overtime_minutes.overtime_fee == 15.0);
    assert(fifteen_overtime_minutes.total_fee == 55.0);

    const auto immediate_slot_booking = parking::fees::calculate(30, 23 * 60, 40.0);
    assert(immediate_slot_booking.billed_hours == 1);
    assert(immediate_slot_booking.base_fee == 40.0);
    assert(immediate_slot_booking.billed_overtime_minutes == 23);
    assert(immediate_slot_booking.overtime_fee == 23.0);
    assert(immediate_slot_booking.total_fee == 63.0);

    const auto overtime_ignores_hourly_rate = parking::fees::calculate(60, 1800, 1000.0);
    assert(overtime_ignores_hourly_rate.overtime_fee == 30.0);

    const auto early_exit = parking::fees::calculate(150, 0, 40.0);
    assert(early_exit.billed_hours == 3);
    assert(early_exit.total_fee == 120.0);

    return 0;
}
