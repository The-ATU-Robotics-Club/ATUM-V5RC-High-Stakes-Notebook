#include "units.hpp"

namespace atum {
degree_t constrain180(const degree_t angle) {
  return degree_t{remainder(getValueAs<degree_t>(angle), 360.0)};
}

double constrainPI(const double angle) {
  return remainder(angle, 2.0 * M_PI);
}

meter_t difference(const meter_t left, const meter_t right) {
  return left - right;
}

radian_t difference(const radian_t left, const radian_t right) {
  return constrain180(left - right);
}
} // namespace atum