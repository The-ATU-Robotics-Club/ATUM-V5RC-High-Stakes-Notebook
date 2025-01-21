/**
 * @file robot.hpp
 * @brief Includes the Robot parent class as well as the macros needed for it to
 * function. In addition there are some useful aliases included.
 * @date 2024-12-23
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../gui/manager.hpp"
#include <functional>
#include <string>

namespace atum {
/**
 * @brief Type alias to improve readability of this file.
 *
 */
using Routine = std::function<void()>;

/**
 * @brief Automatically inserts boilerplate to make use of the Robot class.
 * Should be placed on the line immediately following a derivative of Robot's
 * opening curly brace for its definition.
 *
 */
#define ROBOT_BOILERPLATE()                                                    \
  public:                                                                      \
  friend class Robot;                                                          \
                                                                               \
  private:                                                                     \
  void initializeRoutines()

/**
 * @brief Creates space within which to place the routine definitions the robot
 * will run and the GUI will display.
 *
 */
#define ROUTINE_DEFINITIONS_FOR(robot) void robot::initializeRoutines()

/**
 * @brief Begins an autonomous routine definition. The name parameter will be
 * what is displayed by the routine selector on the GUI.
 *
 */

#define START_ROUTINE(name)                                                    \
  if(!routineNames.empty()) routineNames += '\n';                              \
  routineNames += name;                                                        \
        routines.push_back( \
            [this]() {
/**
 * @brief Ends an autonomous routine definition.
 *
 */
#define END_ROUTINE                                                            \
  });

class Robot {
  public:
  // These are deleted to force the use of the templated constructor.
  Robot() = delete;
  Robot(const Robot &) = delete;
  Robot(Robot &&) = delete;

  /**
   * @brief Constructs the Robot by calling its children's initializeRoutine
   * method. This essentially stores the routine information for future use (and
   * tells the GUI to begin loading).
   *
   * @tparam SpecRobot
   * @param spec
   */
  template <class SpecRobot>
  Robot(SpecRobot *spec) {
    spec->initializeRoutines();
    GUI::Manager::startLoading(spec->getRoutineNames());
  }

  /**
   * @brief The behavior of the robot during the disabled period.
   *
   */
  virtual void disabled() = 0;

  /**
   * @brief The behavior of the robot during the operator control period.
   *
   */
  virtual void opcontrol() = 0;

  /**
   * @brief The behavior of the robot during the autonomous period.
   *
   */
  virtual void autonomous();

  /**
   * @brief Gets the names of the defined autonomous routines.
   *
   * @return std::string
   */
  std::string getRoutineNames();

  protected:
  std::string routineNames;
  std::vector<Routine> routines;
};

} // namespace atum