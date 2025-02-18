#include "atum/time/time.hpp"
#include "limitSwitch.hpp"

namespace atum {
LimitSwitch::LimitSwitch(const std::uint8_t port,
                         int iFilterSize,
                         const Logger::Level loggerLevel) :
    limitSwitch{port},
    filterSize{iFilterSize},
    logger{loggerLevel} {
  wait(adiCalibrationTime);
}

LimitSwitch::LimitSwitch(const ADIExtenderPort &port,
                         int iFilterSize,
                         const Logger::Level loggerLevel) :
    limitSwitch{port()},
    filterSize{iFilterSize},
    logger{loggerLevel} {
  wait(adiCalibrationTime);
}

bool LimitSwitch::isPressed() {
  if(!timer.goneOff()) {
    return debounce();
  }
  const bool newPress{limitSwitch.get_value()};
  presses.push_back(newPress);
  if(presses.size() > filterSize) {
    presses.erase(presses.begin());
  }
  timer.setTime();
  return debounce();
}

bool LimitSwitch::isNewlyPressed() {
  return limitSwitch.get_new_press();
}

bool LimitSwitch::debounce() const {
  if(presses.empty()) {
    return false;
  }
  int numberOfPresses{0};
  for(const bool press : presses) {
    if(press) {
      numberOfPresses++;
    }
  }
  const double percent{numberOfPresses / static_cast<double>(presses.size())};
  return percent > 0.5;
}
} // namespace atum