/**
 * @file profileFollower.hpp
 * @brief Includes the ProfileFollower template class and some helpful aliases.
 * @date 2024-12-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../controllers/controller.hpp"
#include "../utility/acceptable.hpp"
#include "motionProfile.hpp"

namespace atum {
/**
 * @brief These constants are multiplied by the acceleration at each point
 * along the profile (accel when accelerating, decel when decelerating) to
 * better track the profile.
 *
 */
struct AccelerationConstants {
  double accel{0.0};
  double decel{0.0};
};

/**
 * @brief This template class supports easily following a motion profile
 * (lateral or angular depending on the template parameter).
 *
 * Start by tuning without polling position, before tuning the feedforward for
 * the velocity controller, moving onto acceleration constants, before adding in
 * velocity and position feedback.
 *
 * @tparam Unit
 */
template <typename Unit>
class ProfileFollower {
  public:
  // Figure out the corresponding types for the derivative of Unit.
  using UnitsPerSecond = decltype(Unit{1} / 1_s);
  using UnitsPerSecondSq = decltype(UnitsPerSecond{1} / 1_s);
  using UnitProfile = MotionProfile<Unit>;
  using UnitAcceptable = Acceptable<Unit>;

  /**
   * @brief Constructs a new ProfileFollower based on the given parameters.
   *
   * The time out scaling parameter will be used to determine the time out on
   * the profile by multiplying the profile's total time by it. Default is 5%
   * longer than expected.
   *
   * The position control threshold determines how far away from the end of the
   * profile position control is employed. The default will use position control
   * for the entire profile and base it off the current reference rather than
   * the end.
   *
   * @param iProfile
   * @param iAcceptable
   * @param iVelocityController
   * @param iKA
   * @param iPositionController
   * @param iPositionControlThreshold
   * @param iTimeoutScaling
   * @param loggerLevel
   */
  ProfileFollower(const UnitProfile &iProfile,
                  const UnitAcceptable &iAcceptable,
                  std::unique_ptr<Controller> iVelocityController,
                  const AccelerationConstants &iKA,
                  std::unique_ptr<Controller> iPositionController = nullptr,
                  const Unit iPositionControlThreshold = Unit{infinite},
                  const double iTimeoutScaling = 1.05,
                  const Logger::Level loggerLevel = Logger::Level::Info) :
      profile{iProfile},
      acceptable{iAcceptable},
      velocityController{std::move(iVelocityController)},
      kA{iKA},
      positionController{std::move(iPositionController)},
      positionControlThreshold{iPositionControlThreshold},
      timeoutScaling{iTimeoutScaling},
      logger{loggerLevel} {
    if(!velocityController) {
      logger.error("Must provide a velocity controller to profile follower!");
    }
    logger.debug("Profile follower constructed!");
  }

  /**
   * @brief Prepare to follow a new profile defined by the start and end
   * positions. Involves reseting the controllers and acceptable object.
   * Special parameters allow generating the profile with constraints different
   * than the defaults
   *
   * @param start
   * @param iEnd
   * @param specialParams
   */
  void startProfile(const Unit start,
                    const Unit iEnd,
                    const UnitProfile::Parameters specialParams = {}) {
    end = iEnd;
    profile.generate(start, end, specialParams);
    acceptable.reset(timeoutScaling * profile.getTotalTime());
    velocityController->reset();
    if(positionController) {
      positionController->reset();
    }
    logger.debug("Profile follower to follow profile from " + to_string(start) +
                 " to " + to_string(end) + ".");
    if(logger.getLevel() == Logger::Level::Debug) {
      prepareGraphing(start);
    }
  }

  /**
   * @brief Gets the output of the follower based on the current position (s)
   * and velocity (v) readings.
   *
   * @param s
   * @param v
   * @return double
   */
  double getOutput(const Unit s, const UnitsPerSecond v) {
    if(logger.getLevel() == Logger::Level::Debug) {
      graphPoint(s, v);
    }
    const typename UnitProfile::Point reference{profile.getPoint(s)};
    const double positionOutput{getPositionOutput(s, reference.s)};
    const double velocityOutput{
        velocityController->getOutput(getValueAs<UnitsPerSecond>(v),
                                      getValueAs<UnitsPerSecond>(reference.v))};
    double accelerationOutput{getAccelerationOutput(reference.a)};
    acceptable.canAccept(s, end);
    return positionOutput + velocityOutput + accelerationOutput;
  }

  /**
   * @brief Returns true if finished following the profile.
   *
   * @return true
   * @return false
   */
  bool isDone() {
    return acceptable.canAccept();
  }

  private:
  /**
   * @brief Gets the output of the position controller if it is provided. If
   * position control threshold is infinite (default) uses the current
   * reference, otherwise uses the end of the profile.
   *
   * @param state
   * @return double
   */
  double getPositionOutput(const Unit state, const Unit reference) const {
    if(!positionController || abs(end - state) >= positionControlThreshold) {
      return 0.0;
    }
    const double positionError{positionControlThreshold >= Unit{infinite} ?
                                   difference(reference, state) :
                                   difference(end, state)};
    return positionController->getOutput(positionError);
  }

  /**
   * @brief Gets the output of the acceleration constants multiplied by the
   * reference acceleration.
   *
   * @param accel
   * @return double
   */
  double getAccelerationOutput(const UnitsPerSecondSq accel) const {
    double accelerationOutput{getValueAs<UnitsPerSecondSq>(accel)};
    if(accel < UnitsPerSecondSq{0}) {
      accelerationOutput *= profile.isBackwards() ? kA.accel : kA.decel;
    } else {
      accelerationOutput *= profile.isBackwards() ? kA.decel : kA.accel;
    }
    return accelerationOutput;
  }

  /**
   * @brief Prepare the graph screen.
   *
   */
  void prepareGraphing(const Unit start) {
    const typename UnitProfile::Parameters motionParams{
        profile.getParameters()};
    GUI::Graph::clearSeries(GUI::SeriesColor::Green);
    GUI::Graph::clearSeries(GUI::SeriesColor::Cyan);
    const double rawStart{getValueAs<Unit>(start)};
    const double rawEnd{getValueAs<Unit>(end)};
    GUI::Graph::setSeriesRange(
        {std::min(rawStart, rawEnd), std::max(rawStart, rawEnd)},
        GUI::SeriesColor::Green);
    GUI::Graph::setSeriesRange(getValueAs<UnitsPerSecond>(motionParams.maxV),
                               GUI::SeriesColor::Cyan);
  }

  /**
   * @brief Graphs a point.
   *
   * @param p
   */
  void graphPoint(const Unit s, const UnitsPerSecond v) {
    GUI::Graph::addValue(getValueAs<Unit>(s), GUI::SeriesColor::Green);
    GUI::Graph::addValue(getValueAs<UnitsPerSecond>(v), GUI::SeriesColor::Cyan);
  }

  UnitProfile profile;
  UnitAcceptable acceptable;
  std::unique_ptr<Controller> velocityController;
  AccelerationConstants kA;
  std::unique_ptr<Controller> positionController;
  Unit positionControlThreshold;
  const double timeoutScaling;
  Logger logger;
  Unit end;
};

/**
 * @brief Specifically produce profile followers for lateral and angular
 * profiles and make them easy to access.
 *
 */
using LateralProfileFollower = ProfileFollower<meter_t>;
using AngularProfileFollower = ProfileFollower<radian_t>;
} // namespace atum