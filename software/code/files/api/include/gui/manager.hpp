/**
 * @file manager.hpp
 * @brief Includes the Manager class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "graph.hpp"
#include "log.hpp"
#include "map.hpp"
#include "routines.hpp"
#include "screen.hpp"

namespace atum {
namespace GUI {
/**
 * @brief This class deals with general management of the GUI, involving its
 * initialization, loading, the main menu, and the home screen (as well as
 * displaying an error screen).
 *
 */
class Manager : public Screen {
  public:
  /**
   * @brief Initialize the GUI. Has to be ran before making use of any other
   * GUI methods.
   *
   */
  static void initialize();

  /**
   * @brief Switch to the loading screen while the robot sets itself up. Should
   * be ran AFTER initializing.
   *
   * @param routines
   */
  static void startLoading(const std::string &routines);

  /**
   * @brief Go to the home screen from the loading screen. Called when the robot
   * is done with setup.
   *
   */
  static void finishLoading();

  /**
   * @brief Call whenever there is an error. Makes the homescreen display an
   * image indicating an error state.
   *
   */
  static void error();

  /**
   * @brief For me to know and for you to find out...
   *
   */
  static void easteregg();

  private:
  /**
   * @brief Sets up the loading screen.
   *
   */
  static void loadingScreenSetup();

  /**
   * @brief Sets up the home screen, initially with the ATUM logo.
   *
   */
  static void homeScreenSetup();

  /**
   * @brief Sets up the main menu screen, including all of the necessary buttons
   * to the other screens.
   *
   */
  static void mainMenuScreenSetup();

  static lv_obj_t *splashImage;
};
} // namespace GUI
} // namespace atum