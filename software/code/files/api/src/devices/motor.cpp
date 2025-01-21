#include "motor.hpp"

namespace atum {
Motor::Motor(const MotorPortsList &ports,
             const Gearing &iGearing,
             const std::string &iName,
             const Logger::Level loggerLevel) :
    gearing{iGearing}, name{iName}, logger{loggerLevel} {
  for(std::int8_t port : ports) {
    motors.push_back(
        std::make_unique<pros::Motor>(std::abs(port),
                                      gearing.cartridge,
                                      pros::v5::MotorEncoderUnits::degrees));
    enabled.push_back(true);
    directions.push_back((port < 0) ? -1 : 1);
  }
  check();
  logger.debug("The " + name + " motor is constructed!");
}

void Motor::moveVelocity(const revolutions_per_minute_t velocity) {
  check();
  const double rawVelocity{
      getValueAs<revolutions_per_minute_t>(velocity * gearing.ratio)};
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      motors[i]->move_velocity(directions[i] * rawVelocity);
    }
  }
}

void Motor::moveVoltage(double voltage) {
  check();
  voltage *= 1000;
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      motors[i]->move_voltage(directions[i] * voltage);
    }
  }
}

void Motor::brake() {
  check();
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      motors[i]->brake();
    }
  }
}

degree_t Motor::getPosition() const {
  std::vector<degree_t> positions;
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      positions.push_back(degree_t{directions[i] * motors[i]->get_position()});
    }
  }
  return offset + average(positions) / gearing.ratio;
}

revolutions_per_minute_t Motor::getVelocity() const {
  std::vector<revolutions_per_minute_t> velocities;
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      const revolutions_per_minute_t velocity{directions[i] *
                                              motors[i]->get_actual_velocity()};
      velocities.push_back(velocity);
    }
  }
  return average(velocities) / gearing.ratio;
}

revolutions_per_minute_t Motor::getTargetVelocity() const {
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      const std::int32_t targetVelocity{directions[i] *
                                        motors[0]->get_target_velocity()};
      return revolutions_per_minute_t{targetVelocity};
    }
  }
  // Just suppresses warning, if this happens we're in deep anyway.
  return 0_rpm;
}

std::int32_t Motor::getCurrentDraw() const {
  std::vector<std::int32_t> currents;
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      const std::int32_t current{motors[i]->get_current_draw()};
      currents.push_back(current);
    }
  }
  return average(currents);
}

double Motor::getEfficiency() const {
  std::vector<double> efficiencies;
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      const double efficiency{motors[i]->get_efficiency()};
      efficiencies.push_back(efficiency);
    }
  }
  return average(efficiencies);
}

double Motor::getPower() const {
  std::vector<double> powers;
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      const double power{motors[i]->get_power()};
      powers.push_back(power);
    }
  }
  return average(powers);
}

double Motor::getTemperature() const {
  std::vector<double> temperatures;
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      const double temperature{motors[i]->get_temperature()};
      temperatures.push_back(temperature);
    }
  }
  return average(temperatures);
}

double Motor::getTorque() const {
  std::vector<double> torques;
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      const double torque{motors[i]->get_torque()};
      torques.push_back(torque);
    }
  }
  return average(torques);
}

std::int32_t Motor::getVoltage() const {
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      return directions[i] * motors[i]->get_voltage();
    }
  }
  return 0;
}

pros::v5::MotorBrake Motor::getBrakeMode() const {
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      return motors[i]->get_brake_mode();
    }
  }
  return pros::v5::MotorBrake::coast;
}

std::int32_t Motor::getCurrentLimit() const {
  for(std::size_t i{0}; i < motors.size(); i++) {
    if(enabled[i]) {
      return motors[i]->get_current_limit();
    }
  }
  return 0;
}

void Motor::setBrakeMode(const pros::v5::MotorBrake mode) const {
  for(std::size_t i{0}; i < motors.size(); i++) {
    // Regardless of enabled, try to change this setting.
    motors[i]->set_brake_mode(mode);
  }
}

void Motor::setCurrentLimit(const std::int32_t limit) const {
  for(std::size_t i{0}; i < motors.size(); i++) {
    // Regardless of enabled, try to change this setting.
    motors[i]->set_current_limit(limit);
  }
}

void Motor::resetPosition(const degree_t iOffset) {
  offset = iOffset;
  for(std::size_t i{0}; i < motors.size(); i++) {
    // Regardless of enabled, try to change this setting.
    motors[i]->tare_position();
  }
}

revolutions_per_minute_t Motor::getMaxRPM() const {
  revolutions_per_minute_t maxRPM;
  switch(gearing.cartridge) {
    case pros::MotorGears::red: maxRPM = 100_rpm; break;
    case pros::MotorGears::blue: maxRPM = 600_rpm; break;
    default: maxRPM = 200_rpm; break;
  }
  return maxRPM / gearing.ratio;
}

Motor::Gearing Motor::getGearing() const {
  return gearing;
}

bool Motor::check() {
  bool goodEnough{true};
  for(int i{0}; i < motors.size(); i++) {
    std::int8_t port{motors[i]->get_port()};
    enabled[i] = motors[i]->is_installed();
    goodEnough = goodEnough && enabled[i];
    if(!enabled[i]) {
      logger.error("The " + getName(port) + " motor is not installed.");
    } else if(motors[i]->is_over_temp()) {
      logger.warn("The " + getName(port) + " motor is overheating.");
    }
  }
  return goodEnough;
}

std::string Motor::getName(const std::int8_t port) {
  if(name.empty()) {
    return "port " + std::to_string(port);
  }
  return name + " port " + std::to_string(port);
}
} // namespace atum