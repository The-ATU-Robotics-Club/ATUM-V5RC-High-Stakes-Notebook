/**
 * @file adi.hpp
 * @brief Includes the ADI class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../../pros/adi.hpp"
#include "../utility/logger.hpp"

namespace atum {
/**
 * @brief This class acts as a functor to support dynamically initializing
 * ADI devices on an extender and logging.
 *
 */
class ADIExtenderPort {
  public:
  // These are deleted to force the use of the only sensible constructor.
  ADIExtenderPort() = delete;
  ADIExtenderPort(const ADIExtenderPort &) = delete;
  ADIExtenderPort(ADIExtenderPort &&) = delete;

  /**
   * @brief Constructs a new ADIExtenderPort object with the given ports and
   * logging. Provided in the unfortunate case that two or more ADI extenders
   * are required.
   *
   * @param smartPort
   * @param adiPort
   * @param loggerLevel
   */
  ADIExtenderPort(const std::int8_t smartPort,
                  const std::uint8_t adiPort,
                  const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Constructs a new ADIExtenderPort object by dynamically finding the
   * smart port for the extender. Also does logging.
   *
   * @param adiPort
   * @param loggerLevel
   */
  ADIExtenderPort(const std::uint8_t adiPort,
                  const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Unwraps the port to initialize an ADI device.
   *
   * @return pros::adi::ext_adi_port_pair_t
   */
  pros::adi::ext_adi_port_pair_t operator()() const;

  private:
  pros::adi::ext_adi_port_pair_t port;
  Logger logger;
};
} // namespace atum