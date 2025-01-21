/**
 * @file slewRate.hpp
 * @brief Includes the SlewRate class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../utility/logger.hpp"
#include <algorithm>
#include <utility>

namespace atum {
/**
 * @brief Implementation of a slew rate. Used to limit the acceleration of
 * a motor or other system by stepping up output until reaching a desired
 * level.
 *
 */
class SlewRate {
  public:
  /**
   * @brief Constructs a new SlewRate object, with a separate max allowed
   * decrease and increase in output.
   *
   * @param maxDecRate
   * @param maxIncRate
   * @param loggerLevel
   */
  SlewRate(const double maxDecRate,
           const double maxIncRate,
           const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new SlewRate object, with both the max increase and
   * decrease in output set the same.
   *
   * @param rate
   * @param loggerLevel
   */
  SlewRate(const double rate,
           const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Given a desired output, returns a value
   *
   * @param desired
   * @return double
   */
  double slew(const double desired);

  /**
   * @brief Returns the most recently calculated output.
   * For in-class use, standardizes the debug logger output.
   *
   * @return double
   */
  double getOutput();

  private:
  double decRate;
  double incRate;
  Logger logger;
  double output{0};
};
} // namespace atum