/**
 * @file goalClamp.hpp
 * @brief Includes the GoalClamp class.
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "atum/atum.hpp"

namespace atum {
/**
 * @brief This class deals with the logic for if the goal is aligned in the
 * clamp as well as controlling the clamp.
 *
 */
class GoalClamp {
  public:
  /**
   * @brief Constructs a new GoalClamp object.
   *
   * @param iPiston
   * @param iLimitSwitch1
   * @param iLimitSwitch2
   * @param loggerLevel
   */
  GoalClamp(std::unique_ptr<Piston> iPiston,
            std::unique_ptr<LimitSwitch> iLimitSwitch1,
            std::unique_ptr<LimitSwitch> iLimitSwitch2,
            const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Returns true whenever both limit switches are pressed in. If only
   * one switch is provided (or working), will rely on just one. If both are not
   * available, will always return false.
   *
   * @return true
   * @return false
   */
  bool hasGoal();

  /**
   * @brief Extends the clamp piston.
   *
   */
  void clamp();

  /**
   * @brief Retracts the clamp piston.
   *
   */
  void unclamp();

  /**
   * @brief Toggles whether the clamp piston is extended or retracted.
   *
   */
  void toggleClamp();

  /**
   * @brief A condition to be used for scheduling or waiting. Returns a function
   * that, when called, returns true if the clamp has a goal.
   *
   * @param desired
   * @return Condition
   */
  Condition checkHasGoal();

  private:
  std::unique_ptr<Piston> piston;
  std::unique_ptr<LimitSwitch> limitSwitch1;
  std::unique_ptr<LimitSwitch> limitSwitch2;
  Logger logger;
};
} // namespace atum