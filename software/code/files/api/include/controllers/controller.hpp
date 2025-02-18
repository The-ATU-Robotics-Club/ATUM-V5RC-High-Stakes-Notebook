/**
 * @file controller.hpp
 * @brief Includes the Controller parent class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../utility/logger.hpp"

namespace atum {
/**
 * @brief Acts as an interface for control algorithms like PID and TBH.
 *
 */
class Controller {
  public:
  /**
   * @brief Constructs a new Controller object.
   *
   * @param loggerLevel
   */
  Controller(const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Gets the last calculated output. 
   * For in-class use, standardizes the debug logger output. 
   *
   * @return double
   */
  virtual double getOutput();

  /**
   * @brief Gets the output calculated by error.
   *
   * @param error
   * @return double
   */
  virtual double getOutput(const double error) = 0;

  /**
   * @brief Gets the output calculated by a given state and desired reference.
   *
   * @param state
   * @param reference
   * @return double
   */
  virtual double getOutput(const double state, const double reference) = 0;

  /**
   * @brief Resets the controller.
   *
   */
  virtual void reset() = 0;

  protected:
  Logger logger;
  double output;
};
} // namespace atum