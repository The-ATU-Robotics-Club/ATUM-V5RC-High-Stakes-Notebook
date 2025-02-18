/**
 * @file colorSensor.hpp
 * @brief Includes the ColorSensor class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../../pros/optical.hpp"
#include "../time/timer.hpp"
#include "../utility/logger.hpp"
#include <memory>
#include <vector>

namespace atum {
class ColorSensor {
  public:
  /**
   * @brief This should be used whenever polling the color from these
   * sensors, since it results in the best performance.
   *
   */
  static constexpr second_t refreshRate{20_ms};

  /**
   * @brief List of colors that can be recognized. Later this class
   * may make use of templates or a more extensible structure.
   *
   */
  enum class Color { None, Red, Blue };

  /**
   * @brief Given a color, what is its typical hue (center) and the area
   * around that measurement where it is still considered the same color
   * (threshold).
   *
   */
  struct HueField {
    Color color;
    double center;
    double threshold;
  };

  /**
   * @brief Constructs a new color sensor object with port numbers given.
   *
   * Hue fields given should form a partition (i.e., they shouldn't overlap;
   * think of centers and threshold as "angles"), otherwise will choose color
   * based on order given when in the threshold for multiple fields.
   *
   * Also turns on LED to max brightness and disables gestures.
   *
   * By providing ports, reconnecting is supported whenever the device isn't
   * connected at the beginning of the match.
   *
   * @param ports
   * @param iHueFields
   * @param loggerLevel
   */
  ColorSensor(const PortsList &ports,
              const std::vector<HueField> iHueFields,
              Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new color sensor object with ports dynamically found.
   * Minimum amount refers to the acceptable minimum amount of sensors to be
   * found to not trigger a warning (zero sensors will trigger an error).
   *
   * Hue fields given should form a partition (i.e., they shouldn't overlap;
   * think of centers and threshold as "angles"), otherwise will choose color
   * based on order given when in the threshold for multiple fields.
   *
   * Also turns on LED to max brightness and disables gestures.
   *
   * @param expectedAmount
   * @param iHueFields
   * @param loggerLevel
   */
  ColorSensor(const std::size_t expectedAmount,
              const std::vector<HueField> iHueFields,
              Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Gets the detected color. Will return None if outside of proximity
   * threshold given (unless 0). Turns LED on if something is nearby.
   *
   * Also keeps track of the number of objects that have passed by the sensor.
   *
   * @return Color
   */
  Color getColor();

  /**
   * @brief Keeps track of the number of objects that have passed by the sensor.
   *
   */
  int tallyCount();

  /**
   * @brief Gets the number of objects that have passed by the sensor.
   *
   * @return int
   */
  int getCount();

  /**
   * @brief Resets the count for the number of objects that have passed by the
   * sensor.
   *
   */
  void resetCount();

  /**
   * @brief Checks if the color sensor is functioning by seeing if it is
   * installed.
   *
   * @return true
   * @return false
   */
  bool check();

  private:
  /**
   * @brief This is the value get_proximity returns when an object is considered
   * near according to the brain device menu.
   *
   */
  static constexpr int32_t nearProximity{250};

  /**
   * @brief Turns the LED on, sets the integration time, and disables gestures.
   *
   */
  void initializeColorSensor();

  /**
   * @brief Gets the
   *
   * @return Color
   */
  Color rawGetColor();

  std::vector<std::unique_ptr<pros::Optical>> opticals;
  std::vector<HueField> hueFields;
  Logger logger;
  int count{0};
  bool previousNearby{false};
  Color colorReading{Color::None};
};

/**
 * @brief Returns a string representation of the color given.
 *
 * @param color
 * @return std::string
 */
std::string toString(const ColorSensor::Color &color);
} // namespace atum