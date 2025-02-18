/**
 * @file led.hpp
 * @brief Includes the LED class.
 * @date 2025-2-3
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "adi.hpp"


namespace atum {
/**
 * @brief Straightforward wrapper around the PROS LED class to support
 * dynamically finding ADI extenders and easier updating.
 *
 * Default color for LEDs is white. You should generally not use max brightness.
 *
 */
class LED {
  public:
  /**
   * @brief Commonly used colors with the LED are provided as constants.
   *
   */
  static constexpr int red{0xBB0000};
  static constexpr int green{0x00BB00};
  static constexpr int blue{0x0000BB};
  static constexpr int white{0xBBBBBB};

  /**
   * @brief Constructs a new LED based on the given port and the length of the
   * LED string.
   *
   * @param port
   * @param length
   */
  LED(const std::uint8_t port, const std::uint32_t length);

  /**
   * @brief Constructs a new LED based on the given extender port and the
   * length of the LED string.
   *
   * @param port
   * @param length
   */
  LED(const ADIExtenderPort &port, const std::uint32_t length);

  /**
   * @brief Sets the color of the LEDs and turns them on. 
   *
   * @param iColor
   */
  void setColor(const std::uint32_t iColor);

  /**
   * @brief Turns the LEDs on.
   *
   */
  void on();

  /**
   * @brief Turns the LEDs off.
   *
   */
  void off();

  private:
  pros::adi::LED led;
  std::uint32_t color{white};
  Logger logger;
};
} // namespace atum