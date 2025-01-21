/**
 * @file ladybrown.hpp
 * @brief Includes the Ladybrown class.
 * @date 2024-12-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "atum/atum.hpp"

namespace atum {
/**
 * @brief The various states that the ladybrown can be in.
 *
 */
enum class LadybrownState {
  Idle,
  Extending,
  Retracting,
  // Moving to the completely down position.
  Resting, // Associated with a position!
  Loading, // Associated with a position!
  // Moving past the loading position to get ready to score and continue
  // indexing.
  Preparing, // Associated with a position!
  // Moving to the completely extended position before going to the preparing
  // position.
  Scoring, // Associated with a position!
  FinishScoring,
  FullyExtending
};

/**
 * @brief Class to implement the ladybrown for the robot. Includes manual
 * controls as well as macros to move to several important positions.
 * Additionally provides a holding and balancing mechanism, as well as automates
 * piston actuation.
 *
 */
class Ladybrown : public Task, public StateMachine<LadybrownState> {
  TASK_BOILERPLATE(); // Included in all task derivatives for setup.

  public:
  /**
   * @brief Different parameters to customize for the ladybrown to function
   * well.
   *
   */
  struct Parameters {
    double manualVoltage;
    // Below this position, the code for holding and balancing will not be used
    // and downward movement will be prevented.
    degree_t noMovePosition{0_deg};
    // The position at which the pistons will actuate.
    degree_t flippingPosition{0_deg};
    // Map connecting states that involve moving to a position to their
    // corresponding end position (Resting, Loading, Preparing, and Scoring).
    std::unordered_map<LadybrownState, std::optional<degree_t>> statePositions;
    // Amount of time allocated for the pistons to move before the ladybrown
    // moves on.
    second_t pistonDelay{0_s};
    // Constant for overcoming gravity (overestimating will make the arm "float"
    // use lowest value to keep arm up).
    double kG{0.0};
    // Used to hold the arm in place (or move to a position if profile follower
    // failed to do so).
    PID holdController{{}};
    // Used to help balance the left and right arms.
    PID balanceController{{}};
    // Used to limit jerk whenever manual controls are enabled.
    SlewRate manualSlew{0};
  };

  /**
   * @brief Constructs a new Ladybrown object with the provided parameters.
   *
   * @param iLeft
   * @param iRight
   * @param iPiston
   * @param iRotation
   * @param iLine
   * @param iParams
   * @param iFollower
   * @param loggerLevel
   */
  Ladybrown(std::unique_ptr<Motor> iLeft,
            std::unique_ptr<Motor> iRight,
            std::unique_ptr<Piston> iPiston,
            std::unique_ptr<RotationSensor> iRotation,
            std::unique_ptr<LineTracker> iLine,
            const Parameters &iParams,
            std::unique_ptr<AngularProfileFollower> iFollower,
            const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Tells the ladybrown to stop moving and (in manual control) sets the
   * position to hold.
   *
   */
  void stop();

  /**
   * @brief Tells the ladybrown to manually extend.
   *
   */
  void extend();

  /**
   * @brief Tells the ladybrown to manually retract.
   *
   */
  void retract();

  /**
   * @brief Tells the ladybrown to go to its resting position (completely down).
   *
   */
  void rest();

  /**
   * @brief Tells the ladybrown to go to its loading position.
   *
   */
  void load();

  /**
   * @brief Tells the ladybrown to go to its prepared position (past the loading
   * position but not at the scored position).
   *
   */
  void prepare();

  /**
   * @brief Tells the ladybrown to go to its scoring position (completely
   * extended).
   *
   */
  void score();
  
  /**
   * @brief Tells the ladybrown to back up after scoring. Moves to the Preparing
   * position.
   *
   */
  void finishScore();

  /**
   * @brief Tells the ladybrown to extend all the way out (to its scoring
   * position).
   *
   */
  void fullyExtend();

  /**
   * @brief Gets the name state of the closest important position (Resting,
   * Loading, Preparing, or Scoring) or Idle if moving.
   *
   * @return LadybrownState
   */
  LadybrownState getClosestNamedPosition() const;

  /**
   * @brief Returns if the ladybown detects a ring based on the linetracker;
   * always returns false if the ladybrown is down.
   *
   * @return true
   * @return false
   */
  bool hasRing() const;

  /**
   * @brief Returns if the line tracker has had any detected issues (and is
   * therefore not being used).
   *
   * @return true
   * @return false
   */
  bool noRingDetection();

  /**
   * @brief Checks if the ladybrown is ready to score.
   *
   * @return true
   * @return false
   */
  bool readyToScore();

  /**
   * @brief Checks if running the intake would conflict with a ring in the
   * intake.
   *
   * @return true
   * @return false
   */
  bool mayConflictWithIntake();

  private:
  /**
   * @brief Changes the state, sets if the slew rate is enabled, and sets the
   * current hold position based on the new state.
   *
   * @param newState
   * @param newEnableSlew
   */
  void changeState(const LadybrownState newState, const bool newEnableSlew);

  /**
   * @brief Moves to the position associated with a given state, so long as the
   * current state does not change.
   *
   * @param targetState
   */
  void moveTo(const LadybrownState targetState);

  /**
   * @brief Gets the current position of the ladybrown. Currently based solely
   * on the rotation sensor, but included for future-proofing.
   *
   * @return degree_t
   */
  degree_t getPosition() const;

  /**
   * @brief Gets the current angular velocity of the ladybrown by averaging the
   * readings from the motors and rotation sensor.
   *
   * @return degrees_per_second_t
   */
  degrees_per_second_t getVelocity() const;

  /**
   * @brief Ran in the background tasks to extend and retract the piston as the
   * ladybrown passes the flipping position. Can apply a delay when retracting
   * to support slower retracting pistons.
   *
   */
  void handlePiston();

  /**
   * @brief Prevents motors from going further down than its resting position.
   * Also resets motor positions if they aren't functional.
   *
   * @return true
   * @return false
   */
  bool maintainMotors();

  /**
   * @brief Gets the output for holding the ladybrown in place. If not moving,
   * will use the given hold controller. Will always apply a voltage
   * proportional to the cosine of the absolute position of the ladybrown to
   * counteract gravity.
   *
   * @return double
   */
  double getHoldOutput();

  std::unique_ptr<Motor> left;
  std::unique_ptr<Motor> right;
  std::unique_ptr<Piston> piston;
  std::unique_ptr<RotationSensor> rotation;
  std::unique_ptr<LineTracker> line;
  Parameters params;
  std::unique_ptr<AngularProfileFollower> follower;
  Logger logger;
  std::optional<degree_t> holdPosition;
  bool enableSlew{false};

  double voltage;
};
} // namespace atum