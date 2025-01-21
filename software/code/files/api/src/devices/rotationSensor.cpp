#include "rotationSensor.hpp"

namespace atum {
RotationSensor::RotationSensor(const std::int8_t port,
                               const bool reversed,
                               const Logger::Level loggerLevel) :
    logger{loggerLevel} {
  rotationSensor = std::make_unique<pros::Rotation>(port);
  check();
  initializeRotationSensor(reversed);
}

RotationSensor::RotationSensor(const bool reversed,
                               const Logger::Level loggerLevel) :
    logger{loggerLevel} {
  const auto rotationSensors{pros::Rotation::get_all_devices()};
  if(!rotationSensors.size()) {
    logger.error("Rotation sensor not found!");
    rotationSensor = std::make_unique<pros::Rotation>(errorPort);
    return;
  } else if(rotationSensors.size() > 1) {
    logger.warn("Multiple rotation sensors found! Using first port found.");
  }
  rotationSensor =
      std::make_unique<pros::Rotation>(rotationSensors.front().get_port());
  logger.debug("Rotation sensor found on port " +
               std::to_string(rotationSensor->get_port()) + ".");
  initializeRotationSensor(reversed);
}

degree_t RotationSensor::getPosition() {
  check();
  const degree_t reading{rotationSensor->get_angle() / 100.0};
  const degree_t value{offset + reading};
  logger.debug("Rotation sensor position is: " + to_string(value));
  return value;
}

degree_t RotationSensor::getDisplacement() {
  check();
  const degree_t reading{rotationSensor->get_position() / 100.0};
  const degree_t value{offset + reading};
  logger.debug("Rotation sensor displacement is: " + to_string(value));
  return value;
}

degrees_per_second_t RotationSensor::getVelocity() {
  check();
  const double reading{rotationSensor->get_velocity()};
  const degrees_per_second_t value{reading};
  logger.debug("Rotation sensor velocity is: " + to_string(value));
  return value;
}

void RotationSensor::resetDisplacement(const degree_t iOffset) {
  offset = iOffset;
  rotationSensor->reset_position();
}

bool RotationSensor::check() {
  if(doNotUseAgain) {
    return false;
  }
  const bool installed{rotationSensor->is_installed()};
  if(!installed) {
    logger.error("Rotation sensor on port " +
                 std::to_string(rotationSensor->get_port()) +
                 " is not installed!");
  }
  doNotUseAgain = !installed;
  return installed;
}

void RotationSensor::initializeRotationSensor(const bool reversed) {
  rotationSensor->set_data_rate(5);
  rotationSensor->set_reversed(reversed);
  logger.info("Rotation sensor contructed with port " +
              std::to_string(rotationSensor->get_port()) + ".");
}
} // namespace atum