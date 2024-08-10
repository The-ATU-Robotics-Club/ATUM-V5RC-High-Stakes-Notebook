#include "odometer.hpp"

namespace atum {
Odometer::Odometer(const std::uint8_t topPort,
                   const std::uint8_t botPort,
                   const inch_t iWheelCircum,
                   const inch_t iFromCenter,
                   const bool reversed) :
    encoder{topPort, botPort, reversed},
    wheelCircum{iWheelCircum},
    fromCenter{iFromCenter} {
  wait(0.25_s); // Allow time for the encoders initialize.
}

inch_t Odometer::traveled() {
  const int32_t ticks{encoder.get_value()};
  const int32_t tickChange{ticks - prevTicks};
  prevTicks = ticks;
  return ticksToRotations * tickChange * wheelCircum;
}

inch_t Odometer::getFromCenter() const {
  return fromCenter;
}
} // namespace atum