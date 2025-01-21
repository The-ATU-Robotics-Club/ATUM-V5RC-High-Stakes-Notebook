/**
 * @file tbh.hpp
 * @brief Includes the TBH class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../devices/motor.hpp"
#include "controller.hpp"
#include <algorithm>
#include <cmath>

namespace atum {
/**
 * @brief An implementation of a Take-Back-Half controller, a commonly
 * used velocity controller.
 *
 */
class TBH : public Controller {
  public:
  /**
   * @brief Parameters for Take-Back-Half.
   *
   */
  struct Parameters {
    // Similar to the kP constant in a PID controller.
    double kTBH{0};
    // Scales to the reference value to provide a "guess" for the
    // voltage needed. Should be set as
    // ff = important reference / approximate voltage at reference.
    double ff{0};
    // Sets bounds for the output of the TBH controller.
    std::pair<double, double> constraints{-Motor::maxVoltage,
                                          Motor::maxVoltage};
  };

  TBH(const Parameters &iParams,
      const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Gets the output of the controller based solely on error.
   * Will reach reference slower than state and reference counterpart
   * since it can't guess a good feedforward value.
   *
   * @param error
   * @return double
   */
  double getOutput(const double error) override;

  /**
   * Gets the output of the controller based on the current state and
   * a desired reference. This is near-certainly the method you want to use.
   *
   * @param state
   * @param reference
   * @return double
   */
  double getOutput(const double state, const double reference) override;

  /**
   * @brief Reset output at reference value, previous reference, and previous
   * error.
   *
   */
  void reset() override;

  /**
   * @brief Gets the parameters of the TBH controller.
   *
   * @return Parameters
   */
  Parameters getParams() const;

  private:
  /**
   * @brief Special reset to adjust the output at reference by scaling
   * the feedforward value provided by the TBH parameters by the reference.
   * Called whenever the reference changes.
   *
   * @param reference
   * @param error
   */
  void reset(const double reference, const double error);

  const Parameters params;
  double outputAtReference{0};
  double prevReference{0};
  double prevError{0};
};
} // namespace atum