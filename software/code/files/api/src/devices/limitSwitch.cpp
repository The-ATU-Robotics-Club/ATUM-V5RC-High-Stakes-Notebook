#include "limitSwitch.hpp"
#include "atum/time/time.hpp"

namespace atum {
LimitSwitch::LimitSwitch(const std::uint8_t port,
                         const bool performCheck,
                         const Logger::Level loggerLevel) :
    limitSwitch{port}, logger{loggerLevel} {
  wait(adiCalibrationTime);
  if(performCheck && !isPressed()) {
    passedTest = false;
    logger.error("Detected issue with limit switch.");
  }
}

LimitSwitch::LimitSwitch(const ADIExtenderPort &port,
                         const bool performCheck,
                         const Logger::Level loggerLevel) :
    limitSwitch{port()}, logger{loggerLevel} {
  wait(adiCalibrationTime);
  if(performCheck && !isPressed()) {
    passedTest = false;
    logger.error("Detected issue with limit switch.");
  }
}

bool LimitSwitch::isPressed() {
  return limitSwitch.get_value();
}

bool LimitSwitch::isNewlyPressed() {
  return limitSwitch.get_new_press();
}

bool LimitSwitch::check() const {
  return passedTest;
}
} // namespace atum