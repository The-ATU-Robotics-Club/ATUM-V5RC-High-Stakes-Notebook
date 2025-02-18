/**
 * @file units.hpp
 * @brief This file deals with with making the use of the nholthaus
 * dimensional unit library more straightforward, involving declaring new
 * units as well.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../depend/units.h"
#include "misc.hpp"

using namespace units;
using namespace units::math;
using namespace units::constants;
using namespace units::literals;
using namespace units::dimensionless;
using namespace units::length;
using namespace units::angle;
using namespace units::velocity;
using namespace units::angular_velocity;
using namespace units::acceleration;
using namespace units::time;

// Needed for macro to properly disambiguate length, feet, etcetera.
namespace units {
UNIT_ADD(length, tile, tiles, tile, unit<std::ratio<2>, feet>)
UNIT_ADD(velocity,
         inches_per_second,
         inches_per_second,
         in_per_s,
         unit<std::ratio<1, 12>, feet_per_second>)
UNIT_ADD(acceleration,
         inches_per_second_squared,
         inches_per_second_squared,
         in_per_s_sq,
         compound_unit<inch, inverse<squared<seconds>>>)
UNIT_ADD(jerk,
         inches_per_second_cubed,
         inches_per_second_cubed,
         in_per_s_cb,
         compound_unit<inch, inverse<cubed<seconds>>>)
UNIT_ADD(acceleration,
         radians_per_second_squared,
         radians_per_second_squared,
         rad_per_s_sq,
         compound_unit<radian, inverse<squared<seconds>>>)
UNIT_ADD(acceleration,
         degrees_per_second_squared,
         degrees_per_second_squared,
         deg_per_s_sq,
         compound_unit<degree, inverse<squared<seconds>>>)
UNIT_ADD(jerk,
         degrees_per_second_cubed,
         degrees_per_second_cubed,
         deg_per_s_cb,
         compound_unit<degree, inverse<cubed<seconds>>>)
} // namespace units

using namespace units::jerk;

namespace atum {
/**
 * @brief This function allows us to convert units into another form
 * before storing them as a plain floating point value.
 *
 * @tparam T
 * @tparam R
 * @param quantity
 * @return R
 */
template <typename T, typename R = double>
R getValueAs(const T &quantity) {
  return unit_cast<R>(T{quantity});
}

/**
 * @brief Constrains an angle between -180 and 180 degrees.
 *
 * @param angle
 * @return degree_t
 */
degree_t constrain180(const degree_t angle);

/**
 * @brief Constrains an angle between -pi and pi radians. Used with
 * UnwrappedPose, prefer constain180.
 *
 * @param angle
 * @return double
 */
double constrainPI(const double angle);

/**
 * @brief Gets the difference between the left and right values (provided for
 * template purposes).
 *
 * @param left
 * @param right
 * @return meter_t
 */
meter_t difference(const meter_t left, const meter_t right);

/**
 * @brief Gets the minimum difference between the left and right values.
 *
 * @param left
 * @param right
 * @return radian_t
 */
radian_t difference(const radian_t left, const radian_t right);
} // namespace atum