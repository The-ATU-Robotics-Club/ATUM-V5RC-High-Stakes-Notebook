#pragma once

#include "../motion/profileFollower.hpp"
#include "../utility/acceptable.hpp"
#include "../utility/gui.hpp"
#include "drive.hpp"

namespace atum {
class PointAt {
  public:
  PointAt(Drive *iDrive,
          const double iTrackerTrust,
          std::unique_ptr<AngularProfileFollower> iFollower);

  void go(const degree_t target,
          const bool reversed = false,
          const std::optional<AngularMotionProfile::MotionConstraints>
              &specialConstraints = {});

  void go(const Position &target,
          const bool reversed = false,
          const std::optional<AngularMotionProfile::MotionConstraints>
              &specialConstraints = {});

  private:
  radians_per_second_t getVelocity() const;

  Drive *drive;
  const double trackerTrust;
  std::unique_ptr<AngularProfileFollower> follower;
};
} // namespace atum