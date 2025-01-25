/**
 * @file distanceSensor.hpp
 * @brief Includes the DistanceSensor class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../utility/logger.hpp"
#include "../utility/units.hpp"

namespace atum {
class DistanceSensor {
  public:
  /**
   * @brief Constructs a new distance sensor with the given port.
   *
   * By providing a port, reconnecting is supported whenever the device isn't
   * connected at the beginning of the match.
   *
   * @param port
   * @param iThreshold
   * @param loggerLevel
   */
  DistanceSensor(const std::int8_t port,
                 const millimeter_t iThreshold = 0_mm,
                 const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new distance sensor by finding its port dynamically.
   *
   * @param iThreshold
   * @param loggerLevel
   */
  DistanceSensor(const millimeter_t iThreshold = 0_mm,
                 const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Returns the current reading of the distance sensor.
   *
   * @return millimeter_t
   */
  millimeter_t getDistance();

  /**
   * @brief Returns true if detected object is within the given
   * threshold.
   *
   * @return true
   * @return false
   */
  bool closeTo();

  /**
   * @brief Checks if the distance sensor is functioning by seeing if it is
   * installed.
   *
   * @return true
   * @return false
   */
  bool check();

  private:
  // This is the distance that is returned from the distance sensor if no object
  // is detected.
  static constexpr int32_t noObjectDistance{9999};
  std::unique_ptr<pros::Distance> distanceSensor;
  Logger logger;
  const millimeter_t threshold;
};
} // namespace atum