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
#include "../sharedSystems/intake.hpp"
#include "../sharedSystems/ladybrown.hpp"
#include "atum/atum.hpp"
#include "atum/depend/units.h"

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
  static constexpr int ID15{0x9e344900};
  static constexpr int ID24{0x64824900};

  /**
   * @brief Setups the robot, changing the setup depending on what ID is
   * provided.
   *
   * @param iID
   */
  RobotClone(const int iID);

  /**
   * @brief The behavior of the clone bots when disabled. Also starts the
   * background tasks for the intake and the ladybrown.
   *
   */
  void disabled() override;

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
   * @brief Sets up the goal clamp for the 15 inch.
   *
   */
  void goalClampSetup15();

  /**
   * @brief Sets up the objects for autonomous routine usage for the 15 inch.
   *
   */
  void autonSetup15();

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
   * @brief Sets up the goal clamp for the 24 inch.
   *
   */
  void goalClampSetup24();

  /**
   * @brief Sets up the objects for autonomous routine usage for the 24 inch.
   *
   */
  void autonSetup24();

  // Opcontrol helpers.
  /**
   * @brief Deals with printing to the remote.
   *
   */
  void remotePrinting();

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

  /**
   * @brief The actions associated with the end of the double goal rush,
   * regardless of which goal was grabbed first.
   *
   */
  void endDoubleGoalRush();

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
   * This is done to allow dynamic routines depending on if the robot managed to
   * receive a goal or not.
   *
   * @param timeout
   * @return Schedule
   */
  Schedule clampWhenReady(const second_t timeout = 2_s);

  const int id;
  Remote remote;
  std::unique_ptr<Drive> drive;
  std::unique_ptr<GPS> gps;
  std::unique_ptr<Intake> intake;
  std::unique_ptr<Ladybrown> ladybrown;
  std::unique_ptr<GoalClamp> goalClamp;
  std::unique_ptr<Piston> goalRush;
  std::unique_ptr<Piston> goalRushClamp;
  std::unique_ptr<PathFollower> pathFollower;
  std::unique_ptr<Turn> turn;
  Timer matchTimer;
  bool useManualControls{false};
  bool useLadybrownControls{false};
  bool useHangControls{false};
  double speedMultiplier{1.0};
  bool scored{false};
};
} // namespace atum