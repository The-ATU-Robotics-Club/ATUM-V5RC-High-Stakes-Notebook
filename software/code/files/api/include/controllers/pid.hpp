/**
 * @file pid.hpp
 * @brief Includes the PID class.
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
 * @brief Implementation of a PID controller with the option for
 * feedforward control; feedforward control is a scalar of the reference if
 * it is provided.
 *
 */
class PID : public Controller {
  public:
  /**
   * @brief Parameters for a PID controller.
   *
   */
  struct Parameters {
    double kP{0};
    double kI{0};
    double kD{0};
    double ff{0};
    // Threshold of error at which the integral term begins accumulating.
    double threshI{std::numeric_limits<long double>::max()};
    // If ffScaling is true, then feedforward will be scaled by the desired
    // reference. Otherwise, the raw value will simply be added to the total
    // output.
    bool ffScaling{false};
    // Sets bounds for the output of the PID controller.
    std::pair<double, double> constraints{-Motor::maxVoltage,
                                          Motor::maxVoltage};
  };

  /**
   * @brief Constructs a new PID object.
   *
   * @param iParams
   * @param loggerLevel
   */
  PID(const Parameters &iParams,
      const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Calculates and gets the output of the controller based only on
   * error. May experience "derivative kick."
   *
   * @param error
   * @return double
   */
  double getOutput(const double error) override;

  /**
   * @brief Calculates and gets the output of the controller based on the
   * current state and a desired reference. Avoids "derivative kick" by taking
   * the derivative of the state instead of error. Enables scaling feedforward.
   *
   * @param state
   * @param reference
   * @return double
   */
  double getOutput(const double state, const double reference) override;

  /**
   * @brief Resets the integral term, previous state, previous error, and
   * output.
   *
   */
  void reset() override;

  /**
   * @brief Gets the parameters of the PID controller.
   *
   * @return Parameters
   */
  Parameters getParams() const;

  private:
  /**
   * @brief Updates the integral term, resetting the term to zero when
   * error crosses zero. Does not accumulate when error is greater than
   * specified I threshold.
   *
   * @param error
   */
  void updateI(const double error);

  Parameters params;
  double I{0};
  double prevState{0};
  double prevError{0};
};
}; // namespace atum