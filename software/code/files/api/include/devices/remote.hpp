#pragma once

#include "../time/task.hpp"
#include "../utility/units.hpp"
#include "api.h"
#include <queue>

namespace atum {
class Remote : public Task {
  public:
  Remote(pros::controller_id_e_t id, const double iDeadzone = 1000);

  int getLTrigger();

  int getRTrigger();

  std::pair<double, double> getLStick();

  std::pair<double, double> getRStick();

  bool getPress(pros::controller_digital_e_t button);

  bool getHold(pros::controller_digital_e_t button);

  void print(const std::uint8_t line, const std::string &message);

  void rumble(const std::string &pattern);

  std::int32_t getBattery();

  private:
  void taskFn1() override;

  pros::Controller remote;
  const double deadzone;
  std::queue<std::string> row0Queue;
  std::queue<std::string> row1Queue;
  std::queue<std::string> row2Queue;
  pros::Mutex row0QueueMutex;
  pros::Mutex row1QueueMutex;
  pros::Mutex row2QueueMutex;
  const double analogToVolt{maxMotorVoltage / 127.0};
  const std::size_t printQueueSize{3}; // Three as there are three lines.
};
} // namespace atum