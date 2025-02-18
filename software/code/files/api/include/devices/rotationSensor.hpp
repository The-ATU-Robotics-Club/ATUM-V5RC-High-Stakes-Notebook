/**
 * @file rotationSensor.hpp
 * @brief Includes the RotationSensor class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../../pros/rotation.hpp"
#include "../time/time.hpp"
#include "../utility/logger.hpp"
#include <memory>

namespace atum {
/**
 * @brief A simple wrapper around the rotation sensor to support logging,
 * dynamic initialization, and dimensional units.
 *
 */
class RotationSensor {
  public:
  /**
   * @brief Constructs a new rotation sensor based on a given port. Reverses if
   * requested.
   *
   * Unlike other smart port devices, explicit-port construction does NOT
   * provide the opportunity to reconnect whenever the device isn't connected at
   * the beginning of the match.
   *
   * @param port
   * @param reversed
   * @param loggerLevel
   */
  RotationSensor(const std::int8_t port,
                 const bool reversed = false,
                 const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new rotation sensor by finding its port dynamically.
   * Reverses if requested.
   *
   * @param reversed
   * @param loggerLevel
   */
  RotationSensor(const bool reversed = false,
                 const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Gets the absolute position of the rotation sensor from 0 to 360
   * degrees with wrapping.
   *
   * @return degree_t
   */
  degree_t getPosition();

  /**
   * @brief Gets the displacement of the rotation sensor either from when it
   * started or when it was last reset.
   *
   * @return double
   */
  degree_t getDisplacement();

  /**
   * @brief Gets the velocity of the rotation sensor in degrees per second.
   *
   * @return degrees_per_second_t
   */
  degrees_per_second_t getVelocity();

  /**
   * @brief Resets the displacement of the rotation sensor to the given offset
   * (default is zero).
   *
   * @param iOffset
   */
  void resetDisplacement(const degree_t iOffset = 0_deg);

  /**
   * @brief Checks if the rotation sensor is functioning by seeing if it is
   * installed or has ever been uninstalled (since reinitialization is finnicky,
   * so it's better to use another sensor).
   *
   * By providing a port, reconnecting is supported whenever the device isn't
   * connected at the beginning of the match.
   *
   * @return true
   * @return false
   */
  bool check();

  private:
  /**
   * @brief For internal use, sets up the rotation sensor to be reversed if
   * requested and improves data rate.
   *
   * @param reversed
   */
  void initializeRotationSensor(const bool reversed);

  std::unique_ptr<pros::Rotation> rotationSensor;
  Logger logger;
  degree_t offset{0_deg};
  bool doNotUseAgain{false};
};
} // namespace atum