/**
 * @file drive.hpp
 * @brief Includes the Drive class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../devices/motor.hpp"
#include "../pose/tracker.hpp"
#include "../utility/logger.hpp"
#include "../utility/misc.hpp"
#include "api.h"

namespace atum {
/**
 * @brief This class encapsulates all of the logic behind the drive.
 * Including tank and arcade controls, setting brake mode, and tracking
 * pose.
 *
 */
class Drive {
  public:
  /**
   * @brief This struct is used to catalog the relevant dimensions of the
   * drive train.
   *
   */
  struct Geometry {
    // The distance between the left and right drive wheels.
    meter_t track{0_m};
    // The circumference of the drive wheels.
    meter_t circum{0_m};
  };

  /**
   * @brief Constructs a new drive. Requires the left and right tracker to be
   * provided.
   *
   * Make sure to set tracker before performing motions!
   *
   * @param iLeft
   * @param iRight
   * @param iGeometry
   * @param loggerLevel
   */
  Drive(std::unique_ptr<Motor> iLeft,
        std::unique_ptr<Motor> iRight,
        const Geometry &iGeometry,
        const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Sets the tracker for the drive. Should be ran before performing
   * motions.
   *
   * @param iTracker
   */
  void setTracker(std::unique_ptr<Tracker> iTracker);
  
  /**
   * @brief Provides tank controls: the left voltage and right voltage are
   * applied directly to the corresponding sides of the drive.
   *
   * @param leftVoltage
   * @param rightVoltage
   */
  void tank(const double leftVoltage, const double rightVoltage);

  /**
   * @brief Provides arcade controls: the forward voltage makes the drive go
   * forward while the turn voltage causes it to turn.
   *
   * @param forwardVoltage
   * @param turnVoltage
   */
  void arcade(const double forwardVoltage, const double turnVoltage);

  /**
   * @brief Stops both sides of the drive.
   *
   */
  void brake();

  /**
   * @brief Sets the current pose of the drive.
   *
   * @param iPose
   */
  void setPose(const Pose &iPose);

  /**
   * @brief Gets the current pose of the drive.
   *
   * @return Pose
   */
  Pose getPose() const;

  /**
   * @brief Gets the distance traveled by the drive since last called.
   *
   * @return meter_t
   */
  meter_t traveled();

  /**
   * @brief Gets the current velocity of the drive.
   *
   * @return meters_per_second_t
   */
  meters_per_second_t getVelocity() const;

  /**
   * @brief Sets the brake mode of the motors on the drive.
   *
   * @param brakeMode
   */
  void setBrakeMode(const pros::v5::MotorBrake brakeMode);

  /**
   * @brief Gets the brake mode of the motors on the drive.
   *
   * @return pros::v5::MotorBrake
   */
  pros::v5::MotorBrake getBrakeMode() const;

  /**
   * @brief Gets the geometry of the drive.
   *
   * @return Geometry
   */
  Geometry getGeometry() const;

  /**
   * @brief Gets the maximum wheel RPM possible by the drive.
   *
   * @return revolutions_per_minute_t
   */
  revolutions_per_minute_t getMaxRPM() const;

  /**
   * @brief Gets the maximum velocity possible by the drive.
   *
   * @return meters_per_second_t
   */
  meters_per_second_t getMaxVelocity() const;

  /**
   * @brief A condition to be used for scheduling or waiting. Returns a function
   * that, when called, returns true if the drive is within the threshold of the
   * given pose.
   *
   * @param pose
   * @param within
   * @return Condition
   */
  Condition checkIsNear(const Pose pose, const meter_t threshold);

  private:
  std::unique_ptr<Motor> left;
  std::unique_ptr<Motor> right;
  std::unique_ptr<Tracker> tracker;
  const Geometry geometry;
  degree_t previousTraveled{0_deg};
  Logger logger;
};
} // namespace atum