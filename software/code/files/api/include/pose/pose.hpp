/**
 * @file pose.hpp
 * @brief Includes the Pose struct as well as corresponding functions.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../gui/routines.hpp"
#include "../utility/units.hpp"

namespace atum {
struct Pose; // Forward declaration for UnwrappedPose.

/**
 * @brief This is used to "unwrap" the Pose objects for easier manipulation.
 * Generally, Pose should be used over this.
 *
 * All members should be interpreted as their base metric counterparts (meters,
 * meters per second, radians, etcetera).
 *
 */
struct UnwrappedPose {
  UnwrappedPose(const double iX = 0.0,
                const double iY = 0.0,
                const double iH = 0.0,
                const double iV = 0.0,
                const double iA = 0.0,
                const double iOmega = 0.0,
                const double iAlpha = 0.0,
                const double iT = 0.0);
  UnwrappedPose(const Pose &pose);
  double x{0.0};
  double y{0.0};
  double h{0.0};
  double v{0.0};
  double a{0.0};
  double omega{0.0};
  double alpha{0.0};
  double t{0.0};

  /**
   * @brief Returns if the two unwrapped poses are equal to each other in x, y,
   * and heading.
   *
   * @param rhs
   * @return true
   * @return false
   */
  bool operator==(const UnwrappedPose &rhs) const;

  /**
   * @brief Returns if the two unwrapped poses are not equal to each other in x,
   * y, or heading.
   *
   * @param rhs
   * @return true
   * @return false
   */
  bool operator!=(const UnwrappedPose &rhs) const;

  /**
   * @brief Returns a unwrapped pose where the x and y values are the
   * sums of the left and right sides x and y values.
   *
   * @param rhs
   * @return UnwrappedPose
   */
  UnwrappedPose operator+(const UnwrappedPose &rhs) const;

  /**
   * @brief Returns a unwrapped pose where the x and y values are the
   * differences between the left and right sides x and y values.
   *
   * @param rhs
   * @return UnwrappedPose
   */
  UnwrappedPose operator-(const UnwrappedPose &rhs) const;

  /**
   * @brief Flips the unwrapped pose around the x-axis for changing colors.
   *
   */
  void flip();
};

/**
 * @brief Returns a unwrapped pose with the x and y values of the
 * right side by the left side. Ignores heading.
 *
 * @param lhs
 * @param rhs
 * @return UnwrappedPose
 */
UnwrappedPose operator*(const double lhs, const UnwrappedPose &rhs);

/**
 * @brief Returns a unwrapped pose with the x and y values of the
 * left side by the right side. Ignores heading.
 *
 * @param lhs
 * @param rhs
 * @return UnwrappedPose
 */
UnwrappedPose operator*(const UnwrappedPose &lhs, const double rhs);

/**
 * @brief Gets the distance between two given unwrapped poses.
 *
 * @param a
 * @param b
 * @return double
 */
double distance(const UnwrappedPose &a, const UnwrappedPose &b);

/**
 * @brief Gets the angle between the state and reference state as
 * the inverse tangent of the differences of their y's over the
 * difference of their x's.
 *
 * If flip on blue is enabled, will flip the reference if the GUI selects blue.
 *
 * @param state
 * @param reference
 * @param flipOnBlue
 * @return double
 */
double angle(const UnwrappedPose &state,
             UnwrappedPose reference,
             const bool flipOnBlue = false);

/**
 * @brief Returns a string representation of the given unwrapped pose.
 *
 * @param pose
 * @return std::string
 */
std::string toString(const UnwrappedPose &pose);

/**
 * @brief This is the struct used to represent poses: x and y coordinates
 * with a heading. For reference on the coordinate system we use, see the
 * GPS documentation from VEX. Also included (not normally in pose, but here
 * for convenience) are motion properties like velocity and acceleration.
 *
 */
struct Pose {
  Pose(const meter_t iX = 0_m,
       const meter_t iY = 0_m,
       const radian_t iH = 0_rad,
       const meters_per_second_t iV = 0_mps,
       const meters_per_second_squared_t iA = 0_mps_sq,
       const radians_per_second_t iOmega = 0_rad_per_s,
       const radians_per_second_squared_t iAlpha = 0_rad_per_s_sq,
       const second_t iT = 0_s);
  Pose(const UnwrappedPose &unwrappedPose);
  meter_t x{0_m};
  meter_t y{0_m};
  radian_t h{0_rad};
  meters_per_second_t v{0_mps};
  meters_per_second_squared_t a{0_mps_sq};
  radians_per_second_t omega{0_rad_per_s};
  radians_per_second_squared_t alpha{0_rad_per_s_sq};
  second_t t{0_s};

  /**
   * @brief Returns if the two poses are equal to each other in x, y,
   * and heading.
   *
   * @param rhs
   * @return true
   * @return false
   */
  bool operator==(const Pose &rhs) const;

  /**
   * @brief Returns if the two poses are not equal to each other in x, y,
   * or heading.
   *
   * @param rhs
   * @return true
   * @return false
   */
  bool operator!=(const Pose &rhs) const;

  /**
   * @brief Returns a pose where the x and y values are the
   * sums of the left and right sides x and y values.
   *
   * @param rhs
   * @return Pose
   */
  Pose operator+(const Pose &rhs) const;

  /**
   * @brief Returns a pose where the x and y values are the
   * differences between the left and right sides x and y values.
   *
   * @param rhs
   * @return Pose
   */
  Pose operator-(const Pose &rhs) const;

  /**
   * @brief Flips the pose around the x-axis for changing colors.
   *
   */
  void flip();
};

/**
 * @brief Returns a pose with the x and y values of the
 * right side by the left side. Ignores heading.
 *
 * @param lhs
 * @param rhs
 * @return Pose
 */
Pose operator*(const double lhs, const Pose &rhs);

/**
 * @brief Returns a pose with the x and y values of the
 * left side by the right side. Ignores heading.
 *
 * @param lhs
 * @param rhs
 * @return Pose
 */
Pose operator*(const Pose &lhs, const double rhs);

/**
 * @brief Gets the distance between two given poses.
 *
 * @param a
 * @param b
 * @return tile_t
 */
tile_t distance(const Pose &a, const Pose &b);

/**
 * @brief Gets the angle between the state and reference state as
 * the inverse tangent of the differences of their y's over the
 * difference of their x's.
 *
 * If flip on blue is enabled, will flip the reference if the GUI selects blue.
 *
 * @param state
 * @param reference
 * @param flipOnBlue
 * @return degree_t
 */
degree_t
    angle(const Pose &state, Pose reference, const bool flipOnBlue = false);

/**
 * @brief Returns a string representation of the given pose.
 *
 * @param pose
 * @return std::string
 */
std::string toString(const Pose &pose);
} // namespace atum