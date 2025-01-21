/**
 * @file odometry.hpp
 * @brief Includes the Odometry class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../devices/imu.hpp"
#include "../devices/odometer.hpp"
#include "../time/task.hpp"
#include "../utility/units.hpp"
#include "tracker.hpp"

namespace atum {
/**
 * @brief Performs tracking using two odometers perpendicular to eachother.
 * Uses arc estimation and twists for improved accuracy.
 *
 */
class Odometry : public Tracker, public Task {
  TASK_BOILERPLATE(); // Included in all task derivatives for setup.

  public:
  /**
   * @brief Constructs a new Odometry object. Expects an odometer parallel to
   * the direction of travel, an odometer perpendicular to the direction of
   * travel, and an IMU.
   *
   * The startBackgroundTasks() method will have to be called if you expect
   * tracking to be performed in the background.
   *
   * @param iForward
   * @param iSide
   * @param iImu
   * @param loggerLevel
   */
  Odometry(std::unique_ptr<Odometer> iForward,
           std::unique_ptr<Odometer> iSide,
           std::unique_ptr<IMU> iImu,
           Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Updates the current pose as tracked by the odometers.
   * Typically called in the background, but could be manually called.
   *
   * @return Pose
   */
  Pose update() override;

  private:
  /**
   * @brief Checks that dx, dy, and dh are finite, valid values before
   * rotating them appropriately and adding them to the current pose
   * estimate.
   *
   * @param dx
   * @param dy
   * @param dh
   * @return Pose
   */
  Pose integratePose(inch_t dx, inch_t dy, radian_t dh);

  std::unique_ptr<Odometer> forward;
  std::unique_ptr<Odometer> side;
  std::unique_ptr<IMU> imu;
  Timer timer;
};
} // namespace atum