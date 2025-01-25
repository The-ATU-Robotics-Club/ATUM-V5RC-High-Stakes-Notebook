/**
 * @file robotPrototype.hpp
 * @brief Includes the RobotPrototype class.
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "atum/atum.hpp"

namespace atum {
/**
 * @brief Encapsulates the behavior for the various test beds we produce.
 *
 */
class RobotPrototype : public Robot {
  ROBOT_BOILERPLATE(); // Included in all task derivatives for setup.

  public:
  /**
   * @brief Constructs a new RobotPrototype object.
   *
   */
  RobotPrototype();

  /**
   * @brief The behavior of the prototype bot when disabled.
   *
   */
  void disabled() override;

  /**
   * @brief The behavior of the prototype bot during opcontrol.
   *
   */
  void opcontrol() override;

  private:
  Remote remote;
  Motor leftMotors{{1, 2}, Motor::Gearing{pros::v5::MotorGears::green}};
  Motor rightMotors{{3, 4}, Motor::Gearing{pros::v5::MotorGears::green}};
  Motor climbMotors{{9, -10}, Motor::Gearing{pros::v5::MotorGears::green}};
};
} // namespace atum