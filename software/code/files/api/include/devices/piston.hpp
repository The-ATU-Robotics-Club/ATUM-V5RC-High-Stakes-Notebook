/**
 * @file piston.hpp
 * @brief Includes the Piston class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "adi.hpp"

namespace atum {
/**
 * @brief Straightforward wrapper around the PROS pneumatics class to support
 * dynamically finding ADI extenders and logging.
 *
 */
class Piston {
  public:
  /**
   * @brief Constructs a new piston based on the given port. Reversed refers to
   * if the tubing is "reversed."
   *
   * @param port
   * @param reversed
   * @param startExtended
   * @param loggerLevel
   */
  Piston(const std::uint8_t port,
         const bool iReversed = false,
         const bool startExtended = false,
         const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new piston based on the given extender port. Reversed
   * refers to if the tubing is "reversed."
   *
   * @param port
   * @param reversed
   * @param startExtended
   * @param loggerLevel
   */
  Piston(const ADIExtenderPort &port,
         const bool iReversed = false,
         const bool startExtended = false,
         const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Extends the piston.
   *
   */
  void extend();

  /**
   * @brief Retracts the piston.
   *
   */
  void retract();

  /**
   * @brief Toggles the piston.
   *
   */
  void toggle();

  /**
   * @brief Returns if the piston is extended.
   *
   * @return true
   * @return false
   */
  bool isExtended();

  private:
  pros::adi::Pneumatics piston;
  const bool reversed;
  Logger logger;
};
} // namespace atum