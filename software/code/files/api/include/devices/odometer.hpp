#pragma once

#include "../time/time.hpp"
#include "../utility/units.hpp"

namespace atum {
class Odometer {
  public:
  Odometer(const std::uint8_t topPort,
           const std::uint8_t botPort,
           const inch_t iWheelCircum,
           const inch_t iFromCenter,
           const bool reversed = false);

  inch_t traveled();

  inch_t getFromCenter() const;

  private:
  pros::ADIEncoder encoder;
  const inch_t wheelCircum;
  const inch_t fromCenter;
  int32_t prevTicks{0};
  static constexpr double ticksToRotations{1.0 / 1024.0 / 4.0};
};
} // namespace atum