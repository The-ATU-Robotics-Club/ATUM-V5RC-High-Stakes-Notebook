/**
 * @file kinematics.hpp
 * @brief Includes the Kinematics template class and helpful aliases.
 * @date 2024-12-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../utility/units.hpp"

namespace atum {
/**
 * @brief This template class provides several static methods to support common
 * kinematic calculations. It is a template so the same equations do not have to
 * be redefined for angles.
 *
 * @tparam Unit
 */
template <typename Unit>
class Kinematics {
  public:
  // Do not allow the construction of an object.
  Kinematics() = delete;
  Kinematics(const Kinematics &) = delete;
  Kinematics(Kinematics &&) = delete;

  // Figure out the corresponding types for the derivative of Unit.
  using UnitsPerSecond = decltype(Unit{1} / 1_s);
  using UnitsPerSecondSq = decltype(UnitsPerSecond{1} / 1_s);
  using UnitsPerSecondCb = decltype(UnitsPerSecondSq{1} / 1_s);

  /**
   * @brief Calculate the next position based on the given parameters.
   *
   * @param dt
   * @param x0
   * @param v0
   * @param a0
   * @param j
   * @return Unit
   */
  static Unit position(const second_t dt,
                       const Unit x0,
                       const UnitsPerSecond v0 = UnitsPerSecond{0.0},
                       const UnitsPerSecondSq a0 = UnitsPerSecondSq{0.0},
                       const UnitsPerSecondCb j = UnitsPerSecondCb{0.0}) {
    const auto dt2 = dt * dt;
    return x0 + v0 * dt + 0.5 * a0 * dt2 + (1.0 / 6.0) * j * dt2 * dt;
  }

  /**
   * @brief Calculates the next velocity based on the given parameters.
   *
   * @param dt
   * @param v0
   * @param a0
   * @param j
   * @return UnitsPerSecond
   */
  static UnitsPerSecond
      velocity(const second_t dt,
               const UnitsPerSecond v0,
               const UnitsPerSecondSq a0 = UnitsPerSecondSq{0.0},
               const UnitsPerSecondCb j = UnitsPerSecondCb{0.0}) {
    return v0 + a0 * dt + 0.5 * j * dt * dt;
  }

  /**
   * @brief Calculates the next velocity based on the given parameters.
   *
   * @param dx
   * @param v0
   * @param a0
   * @return UnitsPerSecond
   */
  static UnitsPerSecond velocity(const Unit dx,
                                 const UnitsPerSecond v0,
                                 const UnitsPerSecondSq a0 = UnitsPerSecondSq{
                                     0.0}) {
    return v0 + 2.0 * a0 * dx;
  }

  /**
   * @brief Calculates the next acceleration based on the given parameters.
   *
   * @param dt
   * @param a0
   * @param j
   * @return UnitsPerSecondSq
   */
  static UnitsPerSecondSq accel(const second_t dt,
                                const UnitsPerSecondSq a0,
                                const UnitsPerSecondCb j = UnitsPerSecondCb{
                                    0.0}) {
    return a0 + j * dt;
  }
};

/**
 * @brief Specifically produce kinematic equations for lateral and angular
 * kinematics and make them easy to access.
 *
 */
using LateralKinematics = Kinematics<meter_t>;
using AngularKinematics = Kinematics<radian_t>;
} // namespace atum