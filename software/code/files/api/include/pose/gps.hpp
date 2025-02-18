/**
 * @file gps.hpp
 * @brief Includes the GPS class.
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "../../pros/gps.hpp"
#include "tracker.hpp"

namespace atum {
/**
 * @brief This acts as a wrapper around the GPS sensor to support integration
 * with the IMUs and resetting other trackers, as well as dynamic port
 * construction, logging, and mapping.
 *
 */
class GPS {
  public:
  /**
   * @brief This is the recommended amount of time to be at standstill before
   * resetting position with the GPS sensor.
   *
   */
  static constexpr second_t standstillResetTime{500_ms};

  /**
   * @brief Constructs a new GPS with the given port.
   *
   * Heading and pose trust refer to how the GPS readings are weighted in
   * comparison to other measurements upon getting heading or resetting
   * trackers.
   *
   * @param port
   * @param offset
   * @param iHeadingTrust
   * @param iFullPoseTrust
   * @param loggerLevel
   */
  GPS(const std::int8_t port,
      const Pose &offset,
      const double iHeadingTrust = 0.5,
      const double iFullPoseTrust = 1.0,
      const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new GPS by finding its port dynamically.
   *
   * Heading and pose trust refer to how the GPS readings are weighted in
   * comparison to other measurements upon getting heading or resetting
   * trackers.
   *
   * @param offset
   * @param iHeadingTrust
   * @param iFullPoseTrust
   * @param loggerLevel
   */
  GPS(const Pose &offset,
      const double iHeadingTrust = 0.5,
      const double iFullPoseTrust = 1.0,
      const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Sets the current pose. Should be called at the beginning of routines
   * with appropriate values.
   *
   * @param pose
   */
  void setPose(const Pose &pose);

  /**
   * @brief Gets the current pose reading of the GPS, provided mostly for
   * debugging purposes.
   *
   * @return Pose
   */
  Pose getPose();

  /**
   * @brief Resets a given tracker by taking a weighted average with its current
   * measured pose.
   *
   * Will rely entirely on the other reading if the GPS check fails or if the
   * current error is greater than the max error for the GPS.
   *
   * @param tracker
   */
  void resetTracker(Tracker *tracker);

  /**
   * @brief Checks if the GPS is functioning by seeing if it is installed.
   *
   * @return true
   * @return false
   */
  bool check();

  private:
  /**
   * @brief Above this error, the GPS is considered not ready to reset another
   * tracker's position.
   *
   */
  static constexpr double maxError{0.5};

  /**
   * @brief Gets the current heading according to the GPS by taking a weighted
   * average with some other reading.
   *
   * Will rely entirely on the other reading if the GPS check fails.
   *
   * @param other
   * @return degree_t
   */
  degree_t getHeading(const degree_t other);

  /**
   * @brief Initializes the GPS by setting its offset and setting its data rate.
   *
   * @param offset
   */
  void initializeGPS(const UnwrappedPose &offset);

  std::unique_ptr<pros::GPS> gps;
  degree_t headingOffset;
  const double headingTrust;
  const double fullPoseTrust;
  Logger logger;
};
} // namespace atum