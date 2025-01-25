/**
 * @file tracker.hpp
 * @brief Includes the Tracker parent class.
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../time/timer.hpp"
#include "../utility/logger.hpp"
#include "pose.hpp"

namespace atum {
/**
 * @brief This class creates an interface for any system that can
 * do pose tracking.
 *
 */
class Tracker {
  public:
  /**
   * @brief Constructs a new tracker and performs logging.
   *
   * @param loggerLevel
   */
  Tracker(const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief This method should update the current tracked pose.
   * It should be overriden in derivatives of Tracker.
   *
   * @return Pose
   */
  virtual Pose update() = 0;

  /**
   * @brief Sets the current pose of the tracker.
   *
   * @param iPose
   */
  virtual void setPose(const Pose &iPose);

  /**
   * @brief Gets the current pose of the tracker.
   *
   * @return Pose
   */
  virtual Pose getPose();

  protected:
  Logger logger;
  Pose pose;
};
} // namespace atum