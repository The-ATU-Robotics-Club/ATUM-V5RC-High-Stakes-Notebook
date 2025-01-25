#include "colorSensor.hpp"

namespace atum {

ColorSensor::ColorSensor(const std::int8_t port,
                         const std::vector<HueField> iHueFields,
                         const Logger::Level loggerLevel) :
    hueFields{iHueFields}, logger{loggerLevel} {
  colorSensor = std::make_unique<pros::Optical>(port);
  check();
  initializeColorSensor();
}

ColorSensor::ColorSensor(const std::vector<HueField> iHueFields,
                         const Logger::Level loggerLevel) :
    hueFields{iHueFields}, logger{loggerLevel} {
  const auto colorSensors{pros::Optical::get_all_devices()};
  if(!colorSensors.size()) {
    logger.error("Color sensor not found!");
    colorSensor = std::make_unique<pros::Optical>(errorPort);
    return;
  } else if(colorSensors.size() > 1) {
    logger.warn("Multiple color sensors found! Using first port found.");
  }
  colorSensor =
      std::make_unique<pros::Optical>(colorSensors.front().get_port());
  initializeColorSensor();
}

ColorSensor::Color ColorSensor::getColor() {
  tallyCount();
  if(colorSensor->get_proximity() < nearProximity) {
    return Color::None;
  }
  for(const HueField &hueField : hueFields) {
    const double reading{getRawHue()};
    // Have to account for the "angle wrap" here.
    const double difference{remainder(hueField.center - reading, 360.0)};
    if(std::abs(difference) < hueField.threshold) {
      return hueField.color;
    }
  }
  return Color::None;
}

int ColorSensor::tallyCount() {
  if(colorSensor->get_proximity() < nearProximity) {
    previousNearby = false;
  } else if(!previousNearby) {
    previousNearby = true;
    count++;
  }
  return getCount();
}

int ColorSensor::getCount() {
  logger.debug("Color sensor counts " + std::to_string(count) + ".");
  return count;
}

void ColorSensor::resetCount() {
  count = 0;
}

double ColorSensor::getRawHue() {
  check();
  const double reading{colorSensor->get_hue()};
  logger.debug("Color sensor hue reading is " + std::to_string(reading) + ".");
  return reading;
}

bool ColorSensor::check() {
  const bool installed{colorSensor->is_installed()};
  if(!installed) {
    logger.error("Color sensor on port " +
                 std::to_string(colorSensor->get_port()) +
                 " is not installed!");
  }
  return installed;
}

void ColorSensor::initializeColorSensor() {
  // The abundance of delays in here is because of a seeming undocumented
  // "delay" needed for many of these values to be set.
  wait(100_ms);
  colorSensor->set_led_pwm(100);
  wait(100_ms);
  colorSensor->set_integration_time(3);
  wait(100_ms);
  colorSensor->disable_gesture();
  wait(100_ms);
  logger.info("Color sensor constructed with port " +
              std::to_string(colorSensor->get_port()) + ".");
}

std::string toString(const ColorSensor::Color &color) {
  switch(color) {
    case ColorSensor::Color::Red: return "Red";
    case ColorSensor::Color::Blue: return "Blue";
    default: return "None";
  }
}
} // namespace atum