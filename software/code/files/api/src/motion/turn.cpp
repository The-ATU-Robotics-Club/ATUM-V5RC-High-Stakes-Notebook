#include "turn.hpp"

namespace atum {
Turn::Turn(Drive *iDrive,
           std::unique_ptr<AngularProfileFollower> iFollower,
           const Logger::Level loggerLevel) :
    drive{iDrive}, follower{std::move(iFollower)}, logger{loggerLevel} {}

void Turn::toward(const Pose &target,
                  const AngularProfile::Parameters &specialParams) {
  Pose state{drive->getPose()};
  if(flipped) {
    state.flip();
  }
  const degree_t targetAngle{angle(state, target)};
  toward(targetAngle, specialParams);
}

void Turn::toward(degree_t target,
                  const AngularProfile::Parameters &specialParams) {
  interrupted = false;
  if(flipped) {
    target *= -1;
  }
  logger.debug("Turning to " + to_string(target) + ".");
  const degree_t initialHeading{drive->getPose().h};
  const degree_t shortestAngle{constrain180(target - initialHeading)};
  follower->startProfile(
      initialHeading, initialHeading + shortestAngle, specialParams);
  while(!follower->isDone() && !interrupted) {
    const Pose state{drive->getPose()};
    const double output{follower->getOutput(state.h, state.omega)};
    drive->arcade(0, output);
    wait();
  }
  drive->brake();
  if(interrupted) {
    logger.debug("Turn was interrupted!");
    interrupted = false;
  } else {
    logger.debug("Turn complete!");
  }
}

void Turn::awayFrom(const Pose &target,
                    const AngularProfile::Parameters &specialParams) {
  Pose state{drive->getPose()};
  if(flipped) {
    state.flip();
  }
  const degree_t targetAngle{angle(state, target)};
  awayFrom(targetAngle, specialParams);
}

void Turn::awayFrom(const degree_t target,
                    const AngularProfile::Parameters &specialParams) {
  toward(target + 180_deg, specialParams);
}
} // namespace atum