#include "colorSensor.hpp"

namespace atum {
ColorSensor::ColorSensor(const PortsList &ports,
                         const std::vector<HueField> iHueFields,
                         Logger::Level loggerLevel) :
    hueFields{iHueFields},
    logger{loggerLevel} {
  for(const std::uint8_t port : ports) {
    pros::v5::Device device{port};
    if(device.get_plugged_type() == pros::DeviceType::optical) {
      opticals.push_back(std::make_unique<pros::Optical>(port));
      logger.debug("Optical sensor found on port " +
                   std::to_string(device.get_port()) + ".");
    } else {
      logger.warn("Optical sensor at port " + std::to_string(port) +
                  " could not be initialized!");
    }
  }
  if(!opticals.size()) {
    logger.error("No optical sensors found!");
  }
  initializeColorSensor();
}

ColorSensor::ColorSensor(const std::size_t expectedAmount,
                         const std::vector<HueField> iHueFields,
                         Logger::Level loggerLevel) :
    hueFields{iHueFields},
    logger{loggerLevel} {
  const auto rawOpticals{pros::Optical::get_all_devices()};
  if(!rawOpticals.size()) {
    logger.error("No optical sensors found!");
  } else if(rawOpticals.size() < expectedAmount) {
    logger.warn("Number of optical sensors found lower than expected!");
  }
  for(auto optical : rawOpticals) {
    opticals.push_back(std::make_unique<pros::Optical>(optical.get_port()));
    logger.debug("Optical sensor found on port " +
                 std::to_string(optical.get_port()) + ".");
  }
  initializeColorSensor();
}

ColorSensor::Color ColorSensor::getColor() {
  tallyCount();
  return rawGetColor();
}

int ColorSensor::tallyCount() {
  check();
  if(rawGetColor() == Color::None) {
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

bool ColorSensor::check() {
  int bad{0};
  for(auto &optical : opticals) {
    const bool installed{optical->is_installed()};
    if(!installed) {
      logger.error("Optical sensor on port " +
                   std::to_string(optical->get_port()) + " is not installed!");
      bad++;
    }
  }
  return bad < opticals.size();
}

void ColorSensor::initializeColorSensor() {
  // The abundance of delays in here is because of a seeming undocumented
  // "delay" needed for many of these values to be set.
  wait(100_ms);
  for(auto &optical : opticals) {
    optical->set_led_pwm(100);
  }
  wait(100_ms);
  for(auto &optical : opticals) {
    optical->set_integration_time(3);
  }
  wait(100_ms);
  for(auto &optical : opticals) {
    optical->disable_gesture();
  }
  wait(100_ms);
  logger.info("Color sensor constructed.");
}

ColorSensor::Color ColorSensor::rawGetColor() {
  for(auto &optical : opticals) {
    if(!optical->is_installed() || optical->get_proximity() < nearProximity) {
      continue;
    }
    for(const HueField &hueField : hueFields) {
      const double reading{optical->get_hue()};
      // Have to account for the "angle wrap" here.
      const double difference{remainder(hueField.center - reading, 360.0)};
      if(std::abs(difference) < hueField.threshold) {
        return hueField.color;
      }
    }
  }
  return Color::None;
}

std::string toString(const ColorSensor::Color &color) {
  switch(color) {
    case ColorSensor::Color::Red: return "Red";
    case ColorSensor::Color::Blue: return "Blue";
    default: return "None";
  }
}
} // namespace atum