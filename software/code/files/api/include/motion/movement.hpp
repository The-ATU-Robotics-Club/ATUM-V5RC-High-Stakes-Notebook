/**
 * @file movement.hpp
 * @brief Includes the Movement class.
 * @date 2025-02-05
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

namespace atum {
/**
 * @brief Provides common functionality between drive movements.
 *
 */
class Movement {
  public:
  /**
   * @brief Interrupts the current movement. Up to children to reset the
   * interrupted variable.
   *
   */
  void interrupt();

  /**
   * @brief Sets whether the movement targets should be flipped across the
   * x-axis (if the color is changed).
   *
   * @param iFlipped
   */
  static void setFlipped(const bool iFlipped);

  protected:
  static bool flipped;
  bool interrupted{false};
};
} // namespace atum