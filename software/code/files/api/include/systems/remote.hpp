/**
 * @file remote.hpp
 * @brief Includes the Remote class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../devices/motor.hpp"
#include "../time/task.hpp"
#include "../time/time.hpp"
#include "../utility/units.hpp"
#include "api.h"
#include <queue>

namespace atum {
/**
 * @brief This class acts as awrapper around the PROS controller class,
 * greatly simplifying printing to the screen as well as getting certain
 * inputs.
 *
 */
class Remote : public Task {
  TASK_BOILERPLATE(); // Included in all task derivatives for setup.

  public:
  /**
   * @brief This enum class simply provides easier means of specifying
   * the type of remote being constructed.
   *
   */
  enum class Type {
    Master = pros::controller_id_e_t::E_CONTROLLER_MASTER,
    Partner = pros::controller_id_e_t::E_CONTROLLER_PARTNER
  };

  /**
   * @brief This enum class provides a greatly simplified means of specifying
   * button inputs on the remote.
   *
   */
  enum class Button {
    A = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_A,
    B = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_B,
    X = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_X,
    Y = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_Y,
    Up = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_UP,
    Down = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_DOWN,
    Left = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_LEFT,
    Right = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_RIGHT,
    L1 = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_L1,
    L2 = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_L2,
    R1 = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_R1,
    R2 = pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_R2
  };

  /**
   * @brief This struct encapsulates the two axii of a remote joystick
   * together (as they are usually requested together) and gives them
   * an appropriate naming scheme.
   *
   */
  struct StickAxis {
    double x;
    double y;
  };

  /**
   * @brief Constructs a new remote based on the type of remote to be
   * configured.
   *
   * @param type
   * @param loggerLevel
   */
  Remote(const Type type = Type::Master,
         const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Gets the current status of the left triggers: 1 if L1 is held,
   * then -1 if L2 is held, and then 0 otherwise.
   *
   * @return int
   */
  int getLTrigger();

  /**
   * @brief Gets the current status of the right triggers: 1 if R1 is held,
   * then -1 if R2 is held, and then 0 otherwise.
   *
   * @return int
   */
  int getRTrigger();

  /**
   * @brief Gets input from the left joystick, scaled to the max motor voltage.
   * Also applies a simple, standard deadzone.
   *
   * @return StickAxis
   */
  StickAxis getLStick();

  /**
   * @brief Gets input from the right joystick, scaled to the max motor voltage.
   * Also applies a simple, standard deadzone.
   *
   * @return StickAxis
   */
  StickAxis getRStick();

  /**
   * @brief Returns if the specified button has been pressed.
   *
   * @param button
   * @return true
   * @return false
   */
  bool getPress(const Button button);

  /**
   * @brief Returns if the specified button is being held down.
   *
   * @param button
   * @return true
   * @return false
   */
  bool getHold(const Button button);

  /**
   * @brief Given a line on the remote (where the top line is considered 0 and
   * the bottom line is considered 2) and a message, prepares the controller to
   * print the message on that line.
   *
   * @param line
   * @param message
   */
  void print(const std::uint8_t line, const std::string &message);

  /**
   * @brief Tells the remote to rumble (vibrate) based off the desired pattern.
   * The patterns consist of the symbols '-', '.', and ' ' where '-' is a long
   * vibration, '.' is a short vibration, and ' ' is a pause. So, for instance,
   * SOS in morse would be remote.rumble("... --- ...").
   *
   * @param pattern
   */
  void rumble(const std::string &pattern);

  private:
  /**
   * @brief A universal, small deadzone where lower joystick readings will be
   * set to zero. Helps prevent motor stalling.
   *
   */
  static constexpr double deadzone{0.25};

  pros::Controller remote;
  Logger logger;
  std::array<std::queue<std::string>, 3> rowQueues;
  static constexpr double analogToVolt{Motor::maxVoltage / 127.0};
  static const std::string linePadding;
  static constexpr std::size_t printQueueSize{3};
  static constexpr second_t minimumPrintDelay{75_ms};
};
} // namespace atum