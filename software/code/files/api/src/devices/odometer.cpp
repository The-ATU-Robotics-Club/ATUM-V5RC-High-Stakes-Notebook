#include "odometer.hpp"

namespace atum {
Odometer::Odometer(const std::uint8_t topPort,
                   const std::uint8_t botPort,
                   const inch_t iWheelCircum,
                   const inch_t iFromCenter,
                   const bool reversed,
                   const Logger::Level loggerLevel) :
    encoder{topPort, botPort, reversed},
    wheelCircum{iWheelCircum},
    fromCenter{iFromCenter},
    logger{loggerLevel} {
  encoder.reset(); // Clear readings. 
  logger.debug("Odometer on ports " + std::to_string(topPort) + " and " +
               std::to_string(botPort) + " has been constructed.");
}

inch_t Odometer::traveled() {
  const int32_t ticks{getTicks()};
  const int32_t tickChange{ticks - prevTicks};
  prevTicks = ticks;
  return ticksToRotations * tickChange * wheelCircum;
}

inch_t Odometer::getFromCenter() const {
  return fromCenter;
}

int32_t Odometer::getTicks() {
  const int32_t ticks{encoder.get_value()};
  logger.debug("Odometer on ports " +
               std::to_string(get<1>(encoder.get_port())) + " and " +
               std::to_string(get<2>(encoder.get_port())) + " reads " +
               std::to_string(ticks) + ".");
  return ticks;
}
} // namespace atum