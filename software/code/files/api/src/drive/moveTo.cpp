#include "moveTo.hpp"

namespace atum {
MoveTo::MoveTo(Drive *iDrive,
               const double iTrackerTrust,
               std::shared_ptr<PointAt> iPointAt,
               std::unique_ptr<LateralProfileFollower> iFollower,
               std::unique_ptr<Controller> iStraight,
               const inch_t iDistanceToSwitch) :
    drive{iDrive},
    trackerTrust{iTrackerTrust},
    pointAt{iPointAt},
    follower{std::move(iFollower)},
    straight{std::move(iStraight)},
    distanceToSwitch{iDistanceToSwitch} {}

void MoveTo::go(const Position &to,
                const bool reversed,
                const std::optional<LateralMotionProfile::MotionConstraints>
                    &specialConstraints) {
  pointAt->go(to, reversed);
  if(straight) straight->reset();
  const Position initial{drive->getPosition()};
  follower->reset(distance(initial, to), specialConstraints);
  while(const auto step = follower->getOutput(
            distance(initial, drive->getPosition()), getVelocity())) {
    const double straightening{getStraighteningOutput(initial, to, reversed)};
    drive->arcade(step.value() * (reversed ? -1.0 : 1.0), straightening);
    wait(10_ms);
  }
  drive->arcade(0, 0);
}

double MoveTo::getStraighteningOutput(const Position &initial,
                                      const Position &to,
                                      const bool reversed) {
  if(!straight) return 0.0;
  const Position state{drive->getPosition()};
  degree_t targetAngle{reversed ? 180_deg : 0_deg};
  if(distance(state, to) <= distanceToSwitch)
    targetAngle += angle(initial, to);
  else
    targetAngle += angle(state, to);
  const degree_t angularError{constrain180(targetAngle - state.h)};
  return straight->getOutput(getValueAs<degree_t>(angularError));
}

meters_per_second_t MoveTo::getVelocity() const {
  return trackerTrust * drive->getPosition().v +
         (1.0 - trackerTrust) * drive->getLinearVelocity();
}
} // namespace atum