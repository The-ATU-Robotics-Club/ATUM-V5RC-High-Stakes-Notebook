#include "time.hpp"

namespace atum {
second_t time() {
  return millisecond_t{pros::millis()};
}

void wait(const second_t delay) {
  if(delay == 0_s) {
    return;
  }
  std::uint32_t now{pros::millis()};
  const std::uint32_t then{getValueAs<millisecond_t, std::uint32_t>(delay)};
  pros::Task::delay_until(&now, then);
}

void waitUntil(const Condition &condition,
               const second_t timeout,
               const second_t delay) {
  const second_t startTime{time()};
  while((timeout == forever || time() - startTime < timeout) && !condition()) {
    wait(delay);
  }
}
} // namespace atum