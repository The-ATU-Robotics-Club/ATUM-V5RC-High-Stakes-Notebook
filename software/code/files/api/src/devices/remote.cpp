#include "remote.hpp"

namespace atum {
Remote::Remote(const pros::controller_id_e_t id, const double iDeadzone) :
    Task{{"Remote"}}, remote{id}, deadzone{iDeadzone} {
  remote.clear();
  start();
}

int Remote::getLTrigger() {
  if(getHold(pros::E_CONTROLLER_DIGITAL_L1)) return 1;
  if(getHold(pros::E_CONTROLLER_DIGITAL_L2)) return -1;
  return 0;
}

int Remote::getRTrigger() {
  if(getHold(pros::E_CONTROLLER_DIGITAL_R1)) return 1;
  if(getHold(pros::E_CONTROLLER_DIGITAL_R2)) return -1;
  return 0;
}

std::pair<double, double> Remote::getLStick() {
  double x{analogToVolt * remote.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)};
  double y{analogToVolt * remote.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)};
  if(abs(x) < deadzone) x = 0;
  if(abs(y) < deadzone) y = 0;
  return std::make_pair(x, y);
}

std::pair<double, double> Remote::getRStick() {
  double x{analogToVolt * remote.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X)};
  double y{analogToVolt * remote.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)};
  if(abs(x) < deadzone) x = 0;
  if(abs(y) < deadzone) y = 0;
  return std::make_pair(x, y);
}

bool Remote::getPress(const pros::controller_digital_e_t button) {
  return remote.get_digital_new_press(button);
}

bool Remote::getHold(const pros::controller_digital_e_t button) {
  return remote.get_digital(button);
}

void Remote::print(const std::uint8_t line, const std::string &message) {
  switch(line) {
    case 0:
      row0QueueMutex.take(10);
      if(row0Queue.size() < printQueueSize)
        row0Queue.push(message + "                   ");
      row0QueueMutex.give();
      break;
    case 1:
      row1QueueMutex.take(10);
      if(row1Queue.size() < printQueueSize)
        row1Queue.push(message + "                   ");
      row1QueueMutex.give();
      break;
    case 2:
      row2QueueMutex.take(10);
      if(row2Queue.size() < printQueueSize)
        row2Queue.push(message + "                   ");
      row2QueueMutex.give();
      break;
    default: break;
  }
}

void Remote::rumble(const std::string &pattern) {
  remote.rumble(pattern.c_str());
}

std::int32_t Remote::getBattery() {
  return remote.get_battery_capacity();
}

void Remote::taskFn1() {
  while(true) {
    row0QueueMutex.take(10);
    if(row0Queue.size()) {
      const auto output = row0Queue.front();
      row0Queue.pop();
      remote.set_text(0, 0, output);
    }
    row0QueueMutex.give();
    wait(75_ms);
    row1QueueMutex.take(10);
    if(row1Queue.size()) {
      const auto output = row1Queue.front();
      row1Queue.pop();
      remote.set_text(1, 0, output);
    }
    row1QueueMutex.give();
    wait(75_ms);
    row2QueueMutex.take(10);
    if(row2Queue.size()) {
      const auto output = row2Queue.front();
      row2Queue.pop();
      remote.set_text(2, 0, output);
    }
    row2QueueMutex.give();
    wait(75_ms);
  }
}
} // namespace atum
