/**
 * @file log.hpp
 * @brief Includes the Log class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "screen.hpp"

namespace atum {
namespace GUI {
/**
 * @brief This class encapsulates the logic and construction of the log
 * screen of the GUI. The logger acts as the "controller" for logging,
 * while this class deals with displaying information to the screen.
 *
 */
class Log : public Screen {
  public:
  // Friend Manager to give access to screen set up. Not ideal, but
  // straightforward solution.
  friend class Manager;

  /**
   * @brief Writes a message to the screen. Should generally use the Logger
   * class instead of this, but is included for situational purposes.
   *
   * @param msg
   */
  static void write(const std::string &msg);

  private:
  /**
   * @brief This deals with setting up the actual screen. Private to force the
   * proper series of steps for setup.
   *
   */
  static void setupScreen();

  static const std::size_t maxLogLines;
  static lv_obj_t *logTextLabel;
  static std::string logText;
  static std::size_t logLines;
};
} // namespace GUI
} // namespace atum