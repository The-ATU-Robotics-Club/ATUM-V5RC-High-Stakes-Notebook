/**
 * @file robotClone.hpp
 * @brief Includes the RobotClone class.
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "../sharedSystems/goalClamp.hpp"
#include "../sharedSystems/goalRush.hpp"
#include "../sharedSystems/intake.hpp"
#include "../sharedSystems/ladybrown.hpp"
#include "atum/atum.hpp"


namespace atum {
/**
 * @brief This encapsulates all of the behaviors related to the clone bots,
 * differing only in setup where ports (and some parameters) may be changed.
 *
 */
class RobotClone : public Robot {
  ROBOT_BOILERPLATE(); // Included in all task derivatives for setup.

  public:
  /**
   * @brief Constants storing the brain IDs for the 15" and 24" bots.
   *
   */
  static constexpr int ID15{0xe6ad4800};
  static constexpr int ID24{0x7cc04c00};

  /**
   * @brief Setups the robot, changing the setup depending on what ID is
   * provided.
   *
   * @param iID
   */
  RobotClone(const int iID);

  /**
   * @brief The behavior of the clone bots when disabled.
   *
   */
  void disabled() override;

  /**
   * @brief The behavior of the clone bots when in operator control.
   *
   */
  void opcontrol() override;

  private:
  // Setup helpers.
  /**
   * @brief Sets up the drive for the 15 inch.
   *
   */
  void driveSetup15();

  /**
   * @brief Sets up the ladybrown for the 15 inch.
   *
   */
  void ladybrownSetup15();

  /**
   * @brief Sets up the intake for the 15 inch.
   *
   */
  void intakeSetup15();

  /**
   * @brief Sets up the goal clamp and goal rush for the 15 inch.
   *
   */
  void goalSetup15();

  /**
   * @brief Sets up the drive for the 24 inch.
   *
   */
  void driveSetup24();

  /**
   * @brief Sets up the ladybrown for the 24 inch.
   *
   */
  void ladybrownSetup24();

  /**
   * @brief Sets up the intake for the 24 inch.
   *
   */
  void intakeSetup24();

  /**
   * @brief Sets up the goal clamp and goal rush for the 24 inch.
   *
   */
  void goalSetup24();

  /**
   * @brief Sets up the objects for autonomous routine usage.
   *
   */
  void autonSetup();

  // Opcontrol helpers.
  /**
   * @brief Deals with printing to the remote and setting the LEDs in accordance
   * with the clamp.
   *
   */
  void visualFeedback();

  /**
   * @brief Deals with controls in manual mode.
   *
   */
  void manualControls();

  /**
   * @brief Deals with controls in ladybrown mode.
   *
   */
  void ladybrownControls();

  /**
   * @brief Deals with controls in intake mode.
   *
   */
  void intakeControls();

  /**
   * @brief Deals with controls in hang mode.
   *
   */
  void hangControls();

  /**
   * @brief Deals with the various shift keys and buttons to change the state of
   * the robot (going to ladybrown/intake/manual mode, changing brake mode,
   * etcetera).
   *
   */
  void configurationControls();

  // Autonomous helpers.
  /**
   * @brief Contains the behavior for the 15" robot during programming skills.
   *
   */
  void skills15();

  /**
   * @brief Contains the behavior for the 24" robot during programming skills.
   *
   */
  void skills24();

  /**
   * @brief Rushes a goal to the right of the robot. Adjusts as needed depending
   * on the auto.
   *
   * @param extraY
   */
  void rushLeft(const tile_t extraY = 0_tile);

  /**
   * @brief Rushes a goal to the left of the robot. Adjusts as needed depending
   * on the auto.
   *
   * @param extraY
   */
  void rushRight(const tile_t extraY = 0_tile);

  /**
   * @brief Collects the rings in a corner. Changes y-values if the negative
   * corner.
   *
   * @param negative
   */
  void collectCorner(const bool negative);

  /**
   * @brief Contains the behavior for the ending of the negative side routines.
   *
   */
  void endOfNegativeRoutines();

  /**
   * @brief Contains the behavior for the ending of the positive side routines.
   *
   */
  void endOfPositiveRoutines();

  /**
   * @brief Sets up the robot with the appropriate starting pose and flips poses
   * if necessary.
   *
   * @param startingPose
   */
  void setupRoutine(Pose startingPose);

  /**
   * @brief Clamps down whenever a goal is aligned. If the given timeout is
   * exceeded before a goal is found, does nothing (so you should place a manual
   * clamp at the point you expect the goal to be there).
   *
   * @param timeout
   */
  void clampWhenReady(const second_t timeout = 4_s);

  /**
   * @brief Grabs a goal with the goal rush when one is detected. If the given
   * timeout is exceeded before a goal is found, does nothing (so you should
   * place a manual grab at the point you expect the goal to be there).
   *
   * @param timeout
   */
  void goalRushWhenReady(const second_t timeout = 4_s);

  /**
   * @brief Sets the sort out color of the intake to the opposite color
   * selected.
   *
   */
  void setSortToOpposite();

  int id;
  Remote remote;
  std::unique_ptr<Drive> drive;
  std::unique_ptr<Intake> intake;
  std::unique_ptr<Ladybrown> ladybrown;
  std::unique_ptr<GoalClamp> goalClamp;
  std::unique_ptr<GoalRush> goalRush;
  Scheduler scheduler;
  std::unique_ptr<PathFollower> pathFollower;
  std::unique_ptr<MoveTo> moveTo;
  std::unique_ptr<Turn> turn;
  Timer matchTimer;
  bool useManualControls{false};
  bool useLadybrownControls{false};
  bool useHangControls{false};
  double speedMultiplier{1.0};
  bool scored{false};
  bool recentlyUnclamped{false};
};
} // namespace atum