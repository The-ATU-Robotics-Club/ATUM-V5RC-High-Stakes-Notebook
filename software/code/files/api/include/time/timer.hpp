/**
 * @file timer.hpp
 * @brief Includes the Timer class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "time.hpp"


namespace atum {
class Timer {
  public:
  /**
   * @brief Constructs a new timer with an alarm time at which
   * it will be considered to have gone off. The default of forever
   * will make it so that the alarm is never considered to have
   * gone off.
   *
   * @param iAlarmTime
   */
  explicit Timer(const second_t iAlarmTime = forever);

  /**
   * @brief Sets the alarm time, the time it will take for the alarm
   * to go off after being reset.
   *
   * @param iAlarmTime
   */
  void setAlarm(const second_t iAlarmTime);

  /**
   * @brief Resets the timer to a specified time (so that an additional alarm
   * time must pass before the alarm has gone off, or so timeElapsed returns the
   * new time immediately after calling this method); default is 0s.
   *
   * @param newTime
   */
  void setTime(const second_t newTime = 0_s);

  /**
   * @brief Sets the timer to 0s, but only once (until restart is called).
   *
   */
  void start();

  /**
   * @brief Allows for the timer to be started again.
   *
   */
  void restart();

  /**
   * @brief Says if the alarm time has elapsed since last reset is greater
   * than the alarm time; never goes off if alarm time is 0s.
   *
   * @return true
   * @return false
   */
  bool goneOff() const;

  /**
   * @brief Gets the time elapsed since the alarm was created.
   *
   * @return second_t
   */
  second_t timeElapsed() const;

  /**
   * @brief Gets the time that has passed since getDT was last called (or timer
   * was created).
   *
   * @return second_t
   */
  second_t getDT();

  /**
   * @brief A condition to be used for scheduling or waiting. Returns a function
   * that, when called, returns true if the alarm has gone off.
   *
   * @param desired
   * @return Condition
   */
  Condition checkGoneOff() const;

  private:
  second_t startTime;
  second_t alarmTime;
  second_t previousTime;
  bool started{false};
};
} // namespace atum