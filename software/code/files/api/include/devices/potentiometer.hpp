/**
 * @file potentiometer.hpp
 * @brief Includes the Potentiometer class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../time/time.hpp"
#include "adi.hpp"

namespace atum {
/**
 * @brief Very straightforward wrapper on the PROS implementation
 * of the potentiometer. Pretty much provides options to reverse the
 * sensor readings, logging, and automatic calibration.
 *
 */
class Potentiometer {
  public:
  /**
   * @brief Constructs a new potentiometer based on the given port.
   * Will block in order to calibrate the sensor.
   *
   * @param port
   * @param iReversed
   * @param loggerLevel
   */
  Potentiometer(const std::uint8_t port,
                const bool iReversed = false,
                const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new potentiometer based on the given extender port.
   * Will block in order to calibrate the sensor.
   *
   * @param port
   * @param iReversed
   * @param loggerLevel
   */
  Potentiometer(const ADIExtenderPort &port,
                const bool iReversed = false,
                const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Gets the current reading of the potentiometer. Performs logging if
   * requested.
   *
   * @return int32_t
   */
  int32_t getReading();

  private:
  /**
   * @brief Calibrates the potentiometer and blocks for an appropriate amount
   * of time for the process to be complete.
   *
   */
  void calibrate();

  pros::adi::Potentiometer pot;
  const bool reversed;
  Logger logger;
};
} // namespace atum