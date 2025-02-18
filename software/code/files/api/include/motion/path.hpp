/**
 * @file path.hpp
 * @brief Includes the Path class.
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "../pose/pose.hpp"
#include "../time/timer.hpp"
#include "../utility/logger.hpp"

namespace atum {
/**
 * @brief This class encapsulates the generation and sampling of paths (or,
 * technically trajectories, since they feature motion information).
 *
 * Sampling is based not only on time along the path, but employs a binary
 * search in order to integrate position along the path into sampling.
 *
 */
class Path {
  public:
  /**
   * @brief The various parameters that are needed to generate and sample a
   * path.
   *
   */
  struct Parameters {
    /**
     * @brief Constructs a new Parameters object.
     *
     * @param onAndOffRamps
     * @param iMaxV
     * @param iMaxA
     * @param iMaxD
     * @param iTrack
     * @param iSpacing
     * @param iMaxSpacingError
     * @param iBinarySearchScaling
     */
    Parameters(const std::pair<meter_t, meter_t> &onAndOffRamps,
               const meters_per_second_t iMaxV = 0_mps,
               const meters_per_second_squared_t iMaxA = 0_mps_sq,
               const meters_per_second_squared_t iMaxD = 0_mps_sq,
               const meter_t iTrack = 0_m,
               const meter_t iSpacing = 1_in,
               const meter_t iMaxSpacingError = 0.1_in,
               const double iBinarySearchScaling = 0.75);

    /**
     * @brief Constructs a new Parameters object.
     *
     * @param ramp
     * @param iMaxV
     * @param iMaxA
     * @param iMaxD
     * @param iTrack
     * @param iSpacing
     * @param iMaxSpacingError
     * @param iBinarySearchScaling
     */
    Parameters(const meter_t ramp,
               const meters_per_second_t iMaxV = 0_mps,
               const meters_per_second_squared_t iMaxA = 0_mps_sq,
               const meters_per_second_squared_t iMaxD = 0_mps_sq,               const meter_t iTrack = 0_m,
               const meter_t iSpacing = 1_in,
               const meter_t iMaxSpacingError = 0.1_in,
               const double iBinarySearchScaling = 0.75);

    /**
     * @brief Constructs a new Parameters object.
     *
     * @param other
     */
    Parameters(const Parameters &other);

    /**
     * @brief Sets the members of the Parameters object to all those non-zero
     * members of the other given Parameters object.
     *
     * @param other
     * @return Parameters&
     */
    Parameters &operator=(const Parameters &other);

    // The amount the starting points are curved toward the starting direction.
    meter_t onRamp;
    // The amount the end points are curved toward the end direction.
    meter_t offRamp;
    meters_per_second_t maxV{0_mps};
    meters_per_second_squared_t maxA{0_mps_sq};
    // Max deceleration (should usually be the same as maxA).
    meters_per_second_squared_t maxD{0_mps_sq};
    // The distance between the left side of the drivetrain and the right side.
    meter_t track;
    // The spacing between each point.
    meter_t spacing{1_in};
    // Maximum allowed deviation for the spacing between a point.
    meter_t maxSpacingError{0.1_in};
    // Controls where the binary search is partitioned at when spacing points
    // (0.75 seems to generally converge quickly, probably shouldn't change).
    double binarySearchScaling{0.75};
  };

  /**
   * @brief Constructs a new Path based on the given waypoints.
   *
   * @param waypoints
   * @param specialParams
   * @param loggerLevel
   */
  Path(const std::pair<Pose, Pose> &waypoints,
       const std::optional<Parameters> &specialParams = {},
       const Logger::Level loggerLevel = Logger::Level::Debug);

  /**
   * @brief Gets the pose at index i of the path.
   *
   * @param i
   * @return Pose
   */
  Pose getPose(const int i);

  /**
   * @brief Gets the number of poses composing the path.
   *
   * @return int
   */
  int getSize() const;

  /**
   * @brief Gets the Parameters used in generating the previous Path.
   *
   * @return Parameters
   */
  Parameters getParams() const;

  /**
   * @brief Sets the default Parameters for all Paths.
   *
   * @param newParams
   */
  static void setDefaultParams(const Parameters &newParams);

  private:
  /**
   * @brief Generates the points along the path and parameterizes them.
   *
   */
  void generate();

  /**
   * @brief Parameterizes -- determines velocity, time, and acceleration based
   * on constraints -- the path.
   *
   */
  void parameterize();

  /**
   * @brief Begins the parameterizing process by determining the max velocity at
   * each point along the path.
   *
   */
  void beginParameterize();

  /**
   * @brief Finishes the parameterizing process by determining the other
   * measurements related to motion (angular velocity, acceleration, etcetera)
   * along the path.
   *
   */
  void endParameterize();

  /**
   * @brief Adds a point to the path spaced properly away from the previous
   * point (t0 is the lower bound of a binary search to find the next point).
   *
   * @param t0
   * @return double
   */
  double addNextPoint(double t0);

  /**
   * @brief Calculates a point along the path based on the given parameter, t.
   *
   * @param t
   * @return Pose
   */
  Pose getPoint(const double t) const;

  /**
   * @brief Gets the curvature at a point along the path based on the given
   * parameter, t.
   *
   * @param t
   * @return double
   */
  double getCurvature(const double t) const;

  /**
   * @brief Gets the derivative at a point along the path based on the given
   * parameter, t.
   *
   * @param t
   * @return Pose
   */
  Pose getDerivative(const double t) const;

  /**
   * @brief Gets the second derivative at a point along the path based on the
   * given parameter, t.
   *
   * @param t
   * @return Pose
   */
  Pose get2ndDerivative(const double t) const;

  /**
   * @brief Graphs the path.
   *
   */
  void graphPath();

  static Parameters defaultParams;
  Pose start;
  Pose startDirection;
  Pose end;
  Pose endDirection;
  Parameters params;
  Logger logger;
  std::vector<Pose> path;
};
} // namespace atum