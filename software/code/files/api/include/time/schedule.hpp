/**
 * @file schedule.hpp
 * @brief Includes the Schedule class.
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../../pros/misc.hpp"
#include "task.hpp"
#include "timer.hpp"
#include <queue>

namespace atum {
/**
 * @brief This class serves as a way to perform actions once certain conditions
 * are met. Construct an object with the necessary parameters for a schedule
 * item (e.g., Schedule someAction{{...}}; ). Will be interrupted if competition
 * state changes or the object goes out of scope. Actions should be fairly
 * simple.
 *
 */
class Schedule : public Task {
  TASK_BOILERPLATE(); // Included in all task derivatives for setup.

  public:
  /**
   * @brief Useful value to have whenever you are only interested in the timeout
   * action. Does what it says on the tin when called.
   *
   */
  static const std::function<void()> doNothing;

  /**
   * @brief Useful value to have whenever you are only interested in the timeout
   * action. Does what it says on the tin when called.
   *
   */
  static const Condition neverMet;

  /**
   * @brief The necessary parameters for a schedule item. Once the condition is
   * true, the todo method will be ran.
   *
   */
  struct Item {
    const std::string name;
    const Condition condition;
    const std::function<void()> todo;
    // The time before the scheduler gives up and runs timeout action. Zero
    // seconds indicates it will never gives up.
    const second_t timeout{forever};
    // Default timeout action to be nothing.
    const std::function<void()> todoTimeout{doNothing};
  };

  /**
   * @brief Schedules an item with an action to perform once a condition is met.
   *
   * @param iItem
   * @param loggerLevel
   */
  Schedule(const Item &iItem,
           const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Cleans up the scheduled action and stops the background task
   * associated with it.
   *
   */
  ~Schedule();

  private:
  Item item;
  Logger logger;
};
} // namespace atum