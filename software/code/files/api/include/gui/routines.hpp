/**
 * @file routines.hpp
 * @brief Includes the Routines class and the MatchColor enum class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "screen.hpp"
#include <array>

namespace atum {
/**
 * @brief This enumerated value represents the two possible match colors.
 *
 */
enum class MatchColor { Red, Blue };

namespace GUI {
/**
 * @brief This class encapsulates the logic and construction of the routine
 * selector screen of the GUI.
 *
 */
class Routines : public Screen {
  public:
  // Friend Manager to give access to screen set up. Not ideal, but
  // straightforward solution.
  friend class Manager;

  /**
   * @brief Gets the selected routine.
   *
   * @return std::size_t
   */
  static std::size_t selectedRoutine();

  /**
   * @brief Gets the selected color.
   *
   * @return MatchColor
   */
  static MatchColor selectedColor();

  private:
  /**
   * @brief This deals with setting up the actual screen. Private to force the
   * proper series of steps for setup. Takes in a string representation of the
   * available routines and hence must be called when the robot routines have
   * been initialized.
   *
   * @param routines
   */
  static void setupScreen(const std::string routines);

  static lv_obj_t *routineSelections;
  static lv_obj_t *colorSwitch;
};
} // namespace GUI
} // namespace atum