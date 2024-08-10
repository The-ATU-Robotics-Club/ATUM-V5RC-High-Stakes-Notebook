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

// Needed for macro to properly disambigaute length, feet, etcetera.
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
UNIT_ADD(jerk,
         feet_per_second_cubed,
         feet_per_second_cubed,
         ft_per_s_cb,
         compound_unit<feet, inverse<cubed<seconds>>>)
UNIT_ADD(acceleration,
         degrees_per_second_squared,
         degrees_per_second_squared,
         deg_per_s_sq,
         compound_unit<degree, inverse<squared<seconds>>>)
UNIT_ADD(jerk,
         meters_per_second_cubed,
         meters_per_second_cubed,
         m_per_s_cb,
         compound_unit<meter, inverse<cubed<seconds>>>)
UNIT_ADD(jerk,
         degrees_per_second_cubed,
         degrees_per_second_cubed,
         deg_per_s_cb,
         compound_unit<degree, inverse<cubed<seconds>>>)
} // namespace units

using namespace units::jerk;

namespace atum {
template <typename T, typename R = double>
R getValueAs(const T &quantity) {
  return unit_cast<R>(T{quantity});
}

template <>
degree_t average<degree_t>(const std::vector<degree_t> &angles);
}