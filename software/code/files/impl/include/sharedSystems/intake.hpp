/**
 * @file intake.hpp
 * @brief Includes the Intake class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "atum/atum.hpp"
#include "ladybrown.hpp"

namespace atum {
/**
 * @brief The various states that the intake can be in.
 *
 */
enum class IntakeState {
  Idle,
  Intaking,
  Indexing,
  Loading,
  Outtaking,
  Jammed,
  Sorting,
  FinishedLoading
};

/**
 * @brief Class to implement the intake for the robot. Contains basic controls
 * as well as more complex support for color sorting, anti-jam, indexing, and
 * loading.
 *
 */
class Intake : public Task, public StateMachine<IntakeState> {
  TASK_BOILERPLATE(); // Included in all task derivatives for setup.

  public:
  /**
   * @brief Different parameters to customize for the intake to
   * function well.
   *
   */
  struct Parameters {
    revolutions_per_minute_t jamVelocity;
    // Timer to track from when we start intaking to when we check for jams.
    Timer timerUntilJamChecks;
    // The time the intake will run outward when jammed.
    second_t timeUntilUnjammed;
    // The time the intake will run outward when throwing while sorting.
    second_t sortThrowTime;
    // The time the intake will continue turning when it sees a ring and is loading. 
    second_t pressLoadTime;
    // The time the intake will run outward when throwing while finishing
    // loading.
    second_t finishLoadingTime;
    // The time the intake will attempt to perform an action before giving up.
    second_t generalTimeout;
  };

  /**
   * @brief Constructs a new Intake object with the provided parameters.
   *
   * @param iMtr
   * @param iColorSensor
   * @param iLadybrown
   * @param iParams
   * @param loggerLevel
   */
  Intake(std::unique_ptr<Motor> iMtr,
         std::unique_ptr<ColorSensor> iColorSensor,
         Ladybrown *iLadybrown,
         const Parameters &iParams,
         const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Tell the intake to run inward. Can be interrupted
   * by the anti-jam or color sort code.
   *
   */
  void intake();

  /**
   * @brief Tell the intake to run inward until it detects a ring
   * that isn't being sorted. Can be interrupted by the anti-jam
   * or color sort code.
   *
   */
  void index();

  /**
   * @brief Tell the intake to work with the ladybrown to load a ring into the
   * arm.
   *
   */
  void load();

  /**
   * @brief Tell the intake run outward.
   *
   */
  void outtake();

  /**
   * @brief Tell the intake to stop moving.
   *
   */
  void stop();

  /**
   * @brief Sets the color the intake will sort out. Setting to "None" will
   * disable sorting.
   *
   * @param iSortOutColor
   */
  void setSortOutColor(const ColorSensor::Color iSortOutColor);

  /**
   * @brief Gets the color the intake is sorting out.
   *
   * @return ColorSensor::Color
   */
  ColorSensor::Color getSortOutColor() const;

  /**
   * @brief Gets the count for the number of rings that have passed.
   *
   * @return int
   */
  int getCount();

  /**
   * @brief Reset the count for the number of rings that have passed.
   *
   */
  void resetCount();

  private:
  /**
   * @brief This method runs whenever we are intaking. It checks for jams or
   * when to color sort and changes state accordingly.
   *
   */
  void intaking();

  /**
   * @brief This method runs whenever we need to get the intake unjammed. When
   * finished, it goes back to the state it was called from.
   *
   */
  void unjamming();

  /**
   * @brief This method runs whenever we need to sort a ring. When
   * finished, it goes back to an appropriate state (intaking, loading, or
   * indexing). If loading, may override lift controls.
   *
   */
  void sorting();

  /**
   * @brief Runs the intake outward for a time to get the hooks away from the
   * ring, before specifying that we are finished loading by changing state (so
   * we don't repeat the action).
   *
   */
  void finishLoading();

  /**
   * @brief For internal use, doesn't have the check on state before switching
   * to intaking/indexing.
   *
   * @param newState
   */
  void forceIntake(const IntakeState newState);

  /**
   * @brief Checks if the intake should index based on the state, sensor
   * functionality, and if enabled (and potentially where the ladybrown is).
   *
   * @return true
   * @return false
   */
  bool shouldIndex() const;

  /**
   * @brief Checks if the intake should sort based on the state, sensor
   * functionality, and if enabled.
   *
   * @return true
   * @return false
   */
  bool shouldSort() const;

  std::unique_ptr<Motor> mtr;
  std::unique_ptr<ColorSensor> colorSensor;
  Ladybrown *ladybrown;
  Logger logger;
  Parameters params;
  ColorSensor::Color sortOutColor{ColorSensor::Color::Red};
  IntakeState returnState{IntakeState::Intaking};
};
} // namespace atum