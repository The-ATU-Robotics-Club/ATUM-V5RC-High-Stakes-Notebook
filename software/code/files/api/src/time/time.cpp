#include "time.hpp"

namespace atum {
second_t time() {
  return millisecond_t{pros::millis()};
}

void wait(second_t time) {
  if(time == 0_s) return;
  std::uint32_t now{pros::millis()};
  std::uint32_t delay{getValueAs<millisecond_t, std::uint32_t>(time)};
  pros::Task::delay_until(&now, delay);
}

void waitUntil(const std::function<bool()> &condition, const second_t timeout) {
  const second_t startTime{time()};
  while((timeout == 0_s || time() - startTime < timeout) && !condition())
    wait();
}
} // namespace atum