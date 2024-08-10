#include "pointAt.hpp"

namespace atum {
PointAt::PointAt(Drive *iDrive,
                 const double iTrackerTrust,
                 std::unique_ptr<AngularProfileFollower> iFollower) :
    drive{iDrive},
    trackerTrust{iTrackerTrust},
    follower{std::move(iFollower)} {}

void PointAt::go(degree_t target,
                 const bool reversed,
                 const std::optional<AngularMotionProfile::MotionConstraints>
                     &specialConstraints) {
  const degree_t initial{drive->getPosition().h};
  if(reversed) target += 180_deg;
  target = constrain180(target - initial);
  follower->reset(target, specialConstraints);
  while(const auto output = follower->getOutput(
            constrain180(drive->getPosition().h - initial), getVelocity())) {
    drive->arcade(0, output.value());
    wait(10_ms);
  }
  drive->arcade(0, 0);
}

void PointAt::go(const Position &target,
                 const bool reversed,
                 const std::optional<AngularMotionProfile::MotionConstraints>
                     &specialConstraints) {
  const Position state{drive->getPosition()};
  const degree_t targetAngle{angle(state, target)};
  go(targetAngle, reversed, specialConstraints);
}

radians_per_second_t PointAt::getVelocity() const {
  return trackerTrust * drive->getPosition().w +
         (1.0 - trackerTrust) * drive->getAngularVelocity();
}
} // namespace atum