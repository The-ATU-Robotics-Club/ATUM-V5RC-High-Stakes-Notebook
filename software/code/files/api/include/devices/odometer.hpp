/**
 * @file odometer.hpp
 * @brief Includes the Odometer class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../time/time.hpp"
#include "../utility/logger.hpp"
#include "../utility/units.hpp"

namespace atum {
/**
 * @brief This class represents an encoder (here an AMT102-V encoder) with a
 * wheel, used to measure traveled distance. This is an essential component in
 * odometry.
 *
 * Distance from center can be easily determined via a CAD model. Wheel
 * circumference can be measured by getting the raw readings from the encoder
 * and moving the robot a known distance; then, simply work backwards to
 * calculate circumference.
 *
 */
class Odometer {
  public:
  /**
   * @brief Constructs a odometer. Top port and bottom port should
   * probably be right next to eachother on A and B, C and D, E and F,
   * or G and H. ADI extender isn't an option to force using brain ports.
   * Wheel circumference is self-explanatory. The "from center" parameter
   * refers to the distance of the odometer wheel to the center of the robot
   * in the relative x or y direction.
   *
   * @param topPort
   * @param botPort
   * @param iWheelCircum
   * @param iFromCenter
   * @param reversed
   */
  Odometer(const std::uint8_t topPort,
           const std::uint8_t botPort,
           const inch_t iWheelCircum,
           const inch_t iFromCenter,
           const bool reversed = false,
           const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Gets the distance the odometer has traveled since last called.
   *
   * @return inch_t
   */
  inch_t traveled();

  /**
   * @brief Gets the distance from the center of the robot to the center of the
   * odometer.
   *
   * @return inch_t
   */
  inch_t getFromCenter() const;

  /**
   * @brief Gets the raw reading from the encoder. Included for troubleshooting
   * and tuning purposes.
   *
   * @return int32_t
   */
  int32_t getTicks();

  private:
  /**
   * @brief This constant represents the conversion from raw encoder ticks to
   * rotations of the wheel. It is specific to the encoders we are using
   * (AMT102-V) and the resolution we are using (2nd from the right DIP turned
   * up, rest turned down).
   *
   */
  static constexpr double ticksToRotations{1.0 / 1024.0 / 4.0};

  pros::adi::Encoder encoder;
  const inch_t wheelCircum;
  const inch_t fromCenter;
  int32_t prevTicks{0};
  Logger logger;
};
} // namespace atum