/**
 * @file goalRush.hpp
 * @brief Includes the GoalRush class.
 * @date 2025-01-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "atum/atum.hpp"

namespace atum {
/**
 * @brief This class encapsulates the logic behind the goal rush mechanism.
 *
 */
class GoalRush {
  public:
  /**
   * @brief Constructs a new GoalRush object.
   *
   * @param iArm
   * @param iClamp
   * @param loggerLevel
   */
  GoalRush(std::unique_ptr<Piston> iArm,
           std::unique_ptr<Piston> iClamp,
           const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Extends the arm of the goal rush.
   *
   */
  void extendArm();

  /**
   * @brief Retracts the arm of the goal rush.
   *
   */
  void retractArm();

  /**
   * @brief Grabs a goal.
   *
   */
  void grab();

  /**
   * @brief Releases a goal.
   *
   */
  void release();

  /**
   * @brief Toggles whether the arm is down or up.
   *
   */
  void toggleArm();

  /**
   * @brief Toggles whether the goal is grabbed or not.
   *
   */
  void toggleClamp();

  private:
  std::unique_ptr<Piston> arm;
  std::unique_ptr<Piston> clamp;
  Logger logger;
};
} // namespace atum
