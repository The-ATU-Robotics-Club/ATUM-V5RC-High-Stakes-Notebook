/**
 * @file limitSwitch.hpp
 * @brief Includes the LimitSwitch class.
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "../time/timer.hpp"
#include "../utility/logger.hpp"
#include "adi.hpp"


namespace atum {
/**
 * @brief A wrapper around the VEX limit switch to support dynamic
 * port initialization and to provide a way to check the sensor is
 * working at the beginning of a match.
 *
 */
class LimitSwitch {
  public:
  /**
   * @brief Constructs a new limit switch based on the given port.
   *
   * Filter size refers to how many presses are counted while determining if the
   * switch is down or not; used for debouncing purposes.
   *
   * @param port
   * @param iFilterSize
   * @param loggerLevel
   */
  LimitSwitch(const std::uint8_t port,
              int iFilterSize = 1,
              const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new limit switch by detecting the port for the sensor.
   *
   * Filter size refers to how many presses are counted while determining if the
   * switch is down or not; used for debouncing purposes.
   *
   * @param port
   * @param filterSize
   * @param loggerLevel
   */
  LimitSwitch(const ADIExtenderPort &port,
              int iFilterSize = 1,
              const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Returns if the limit switch is pressed.
   *
   * @return true
   * @return false
   */
  bool isPressed();

  /**
   * @brief Returns if the limit switch has been newly pressed.
   *
   * @return true
   * @return false
   */
  bool isNewlyPressed();

  private:
  /**
   * @brief Returns true if more than fifty percent of the values in presses are
   * true.
   *
   * @return true
   * @return false
   */
  bool debounce() const;

  pros::adi::DigitalIn limitSwitch;
  int filterSize;
  std::vector<bool> presses;
  Timer timer{10_ms};
  Logger logger;
};
} // namespace atum