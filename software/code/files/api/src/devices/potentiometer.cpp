#include "potentiometer.hpp"

namespace atum {
Potentiometer::Potentiometer(const std::uint8_t port,
                             const bool iReversed,
                             const Logger::Level loggerLevel) :
    pot{port},
    reversed{iReversed},
    logger{loggerLevel} {
  calibrate();
}

Potentiometer::Potentiometer(const ADIExtenderPort &port,
                             const bool iReversed,
                             const Logger::Level loggerLevel) :
    pot{port()},
    reversed{iReversed},
    logger{loggerLevel} {
  calibrate();
}

int32_t Potentiometer::getReading() {
  double reading{pot.get_value_calibrated()};
  if(reversed) {
    reading *= -1;
  }
  logger.debug("Potentiometer is reading " + std::to_string(reading) + ".");
  return reading;
}

void Potentiometer::calibrate() {
  pot.calibrate();
  // Give time to calibrate the sensor.
  wait(adiCalibrationTime);
  logger.debug("Potentiometer has been constructed.");
}
} // namespace atum