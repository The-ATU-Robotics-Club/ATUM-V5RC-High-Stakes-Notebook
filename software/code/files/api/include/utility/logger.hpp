/**
 * @file logger.hpp
 * @brief Includes the Logger class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../../pros/rtos.hpp"
#include "../gui/log.hpp"
#include "../gui/manager.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <vector>

namespace atum {

/**
 * @brief This class handles logging to the terminal, SD card, and log screen.
 * Messages are displayed depending on the provided logger level. If the logger
 * is initialized with the "Info" level, for instance, info, warn, and error
 * messages will be logged.
 *
 */
class Logger {
  public:
  /**
   * @brief Various levels for logging. Controls what messages are logged.
   *
   */
  enum class Level { Debug = 4, Info = 3, Warn = 2, Error = 1, Off = 0 };

  /**
   * @brief Constructs a new Logger object.
   *
   * @param iLevel
   */
  Logger(const Level iLevel = Level::Info);

  /**
   * @brief Log message if logger level is debug.
   *
   * @param msg
   */
  void debug(const std::string &msg);

  /**
   * @brief Log message if logger level is info or higher.
   *
   * @param msg
   */
  void info(const std::string &msg);

  /**
   * @brief Log message if logger level is warn or higher.
   *
   * @param msg
   */
  void warn(const std::string &msg);

  /**
   * @brief Log message if logger level is error or higher. Also trigger error
   * screen.
   *
   * @param msg
   */
  void error(const std::string &msg);

  /**
   * @brief Gets the logger level.
   *
   * @return Level
   */
  Level getLevel() const;

  private:
  // Allow Logger to use writeTo.
  friend void GUI::Log::write(const std::string &msg);

  /**
   * @brief Adds prefix to the message, and outputs to the terminal, file, and
   * screen depending on the message level.
   *
   * @param prefix
   * @param msg
   * @param msgLevel
   */
  void log(const std::string &prefix, const std::string &msg, Level msgLevel);

  /**
   * @brief Checks if the message has already been printed.
   *
   * @param msg
   * @return true
   * @return false
   */
  bool alreadyLogged(const std::string &msg);

  static const std::string logFilename;
  static bool beganLogging;
  static pros::Mutex logMutex;

  Level level;

  std::vector<std::string> logs;
};

} // namespace atum