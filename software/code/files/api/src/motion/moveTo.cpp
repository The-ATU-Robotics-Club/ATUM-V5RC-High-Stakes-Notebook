#include "moveTo.hpp"


namespace atum {
MoveTo::MoveTo(Drive *iDrive,
               Turn *iTurn,
               std::unique_ptr<LateralProfileFollower> iFollower,
               std::unique_ptr<PID> iDirectionController,
               const meter_t iTurnToThreshold,
               const Logger::Level loggerLevel) :
    drive{iDrive},
    turn{iTurn},
    follower{std::move(iFollower)},
    directionController{std::move(iDirectionController)},
    turnToThreshold{iTurnToThreshold},
    logger{loggerLevel} {}

void MoveTo::forward(Pose target,
                     const LateralProfile::Parameters &specialParams) {
  turn->toward(target);
  moveToPoint(target, specialParams, false);
}

void MoveTo::reverse(Pose target,
                     const LateralProfile::Parameters &specialParams) {
  turn->awayFrom(target);
  moveToPoint(target, specialParams, true);
}

void MoveTo::moveToPoint(Pose target,
                         const LateralProfile::Parameters &specialParams,
                         const bool reversed) {
                          interrupted = false;
  directionController->reset();
  if(flipped) {
    target.flip();
  }
  logger.debug("Moving to " + toString(target) + ".");
  const Pose initialPose{drive->getPose()};
  const degree_t linearH{angle(initialPose, target)};
  follower->startProfile(0_m, distance(initialPose, target), specialParams);
  while(!follower->isDone() && !interrupted) {
    const Pose pose{drive->getPose()};
    const meters_per_second_t v{abs(drive->getVelocity())};
    const meter_t traveled{distance(initialPose, pose)};
    double moveOutput{follower->getOutput(traveled, v)};
    degree_t targetH{(distance(pose, target) < turnToThreshold) ?
                         linearH :
                         angle(pose, target)};
    if(reversed) {
      moveOutput *= -1;
      targetH += 180_deg;
    }
    const double hError{getValueAs<degree_t>(constrain180(targetH - pose.h))};
    const double directionOutput{directionController->getOutput(hError)};
    drive->arcade(moveOutput, directionOutput);
    wait();
  }
  drive->brake();
  if(interrupted) {
    logger.debug("Move to was interrupted!");
    interrupted = false;
  } else {
    logger.debug("Move to complete!");
  }
}
} // namespace atum