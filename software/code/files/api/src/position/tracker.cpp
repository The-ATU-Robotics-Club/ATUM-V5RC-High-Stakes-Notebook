#include "tracker.hpp"

namespace atum {
Tracker::Tracker(std::unique_ptr<Logger> iLogger) :
    logger{std::move(iLogger)} {}

void Tracker::setPosition(const Position &iPosition) {
  positionMutex.take(10);
  position = iPosition;
  positionMutex.give();
}

Position Tracker::getPosition() {
  positionMutex.take(10);
  Position current = position;
  positionMutex.give();
  return current;
}

std::pair<meters_per_second_t, radians_per_second_t>
    Tracker::getVW(const inch_t distance, const radian_t dh) {
  second_t dt{timer.timeElapsed()};
  if(!dt) return make_pair(0_in_per_s, 0_rad_per_s);
  auto vw = make_pair(distance / dt, dh / dt);
  timer.resetAlarm();
  return vw;
}
} // namespace atum