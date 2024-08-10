#include "potentiometer.hpp"

namespace atum {
Potentiometer::Potentiometer(const std::uint8_t port) : pot{port} {
  pot.calibrate();
  wait(0.5_s); // Wait for sensor to calibrate.
}

Potentiometer::Potentiometer(const pros::ext_adi_port_pair_t port) : pot{port} {
  pot.calibrate();
  wait(0.5_s); // Wait for sensor to calibrate.
}

degree_t Potentiometer::getPosition() const {
  const double reading{std::abs(pot.get_value_calibrated())};
  return potMaxAngle * reading / 4095.0;
}

degrees_per_second_t Potentiometer::getVelocity() {
  const second_t currentTime{time()};
  const second_t timeDifference{currentTime - previousTime};
  previousTime = currentTime;
  const degree_t currentPosition{getPosition()};
  const degree_t positionDifference{currentPosition - previousPosition};
  previousPosition = currentPosition;
  return positionDifference / timeDifference;
}

} // namespace atum