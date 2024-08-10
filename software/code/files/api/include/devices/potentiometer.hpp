#pragma once

#include "../time/time.hpp"
#include "../utility/units.hpp"
#include "api.h"
#include <queue>

namespace atum {
class Potentiometer {
  public:
  Potentiometer(const std::uint8_t port);

  Potentiometer(const pros::ext_adi_port_pair_t port);

  degree_t getPosition() const;

  degrees_per_second_t getVelocity();

  private:
  pros::ADIPotentiometer pot;
  degree_t previousPosition{0_deg};
  second_t previousTime{0_s};

  static constexpr degree_t potMaxAngle{333_deg};
};
} // namespace atum