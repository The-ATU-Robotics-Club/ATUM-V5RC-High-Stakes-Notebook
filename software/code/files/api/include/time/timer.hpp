#pragma once

#include "time.hpp"

namespace atum {
class Timer {
  public:
  explicit Timer(const second_t &iAlarmTime = 0_s);

  void setAlarmTime(const second_t &iAlarmTime);

  void resetAlarm();

  bool goneOff() const;

  second_t timeElapsed() const;

  private:
  second_t startTime;
  second_t alarmTime;
};
} // namespace atum