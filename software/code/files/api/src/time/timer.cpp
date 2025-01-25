#include "timer.hpp"

namespace atum {
Timer::Timer(const second_t iAlarmTime) :
    startTime{time()},
    alarmTime{iAlarmTime} {}

void Timer::setAlarm(const second_t iAlarmTime) {
  alarmTime = iAlarmTime;
  setTime();
}

void Timer::setTime(const second_t newTime) {
  startTime = time() - newTime;
}

void Timer::start() {
  if(started) {
    return;
  }
  setTime();
  started = true;
}

void Timer::restart() {
  started = false;
  setTime();
}

bool Timer::goneOff() const {
  return timeElapsed() >= alarmTime;
}

second_t Timer::timeElapsed() const {
  return time() - startTime;
}

Condition Timer::checkGoneOff() const {
  return [=]() { return goneOff(); };
}
} // namespace atum