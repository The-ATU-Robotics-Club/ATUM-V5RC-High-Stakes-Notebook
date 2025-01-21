#include "remote.hpp"

namespace atum {
Remote::Remote(const Type type, const Logger::Level loggerLevel) :
    Task{this, loggerLevel},
    remote{static_cast<pros::controller_id_e_t>(type)},
    logger{loggerLevel} {
  remote.clear();
  logger.debug("Remote is constructed!");
  startBackgroundTasks();
}

int Remote::getLTrigger() {
  if(getHold(Button::L1)) {
    return 1;
  }
  if(getHold(Button::L2)) {
    return -1;
  }
  return 0;
}

int Remote::getRTrigger() {
  if(getHold(Button::R1)) {
    return 1;
  }
  if(getHold(Button::R2)) {
    return -1;
  }
  return 0;
}

Remote::StickAxis Remote::getLStick() {
  double x{analogToVolt * remote.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)};
  double y{analogToVolt * remote.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)};
  if(std::abs(x) < deadzone) {
    x = 0;
  }
  if(std::abs(y) < deadzone) {
    y = 0;
  }
  return {x, y};
}

Remote::StickAxis Remote::getRStick() {
  double x{analogToVolt * remote.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X)};
  double y{analogToVolt * remote.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)};
  if(std::abs(x) < deadzone) {
    x = 0;
  }
  if(std::abs(y) < deadzone) {
    y = 0;
  }
  return {x, y};
}

bool Remote::getPress(const Button button) {
  const auto prosButton = static_cast<pros::controller_digital_e_t>(button);
  return remote.get_digital_new_press(prosButton);
}

bool Remote::getHold(const Button button) {
  const auto prosButton = static_cast<pros::controller_digital_e_t>(button);
  return remote.get_digital(prosButton);
}

void Remote::print(const std::uint8_t line, const std::string &message) {
  if(rowQueues[line].size() < printQueueSize) {
    rowQueues[line].push(message + linePadding);
  }
}

void Remote::rumble(const std::string &pattern) {
  remote.rumble(pattern.c_str());
}

TASK_DEFINITIONS_FOR(Remote) {
  START_TASK("Print Handler")
  while(true) {
    for(std::size_t line{0}; line < 3; line++) {
      if(rowQueues[line].size()) {
        const auto output = rowQueues[line].front();
        rowQueues[line].pop();
        remote.set_text(line, 0, output);
      }
      wait(minimumPrintDelay);
    }
  }
  END_TASK
}

const std::string Remote::linePadding{"                   "};
} // namespace atum
