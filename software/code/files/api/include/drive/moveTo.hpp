#pragma once

#include "pointAt.hpp"

namespace atum {
class MoveTo {
  public:
  MoveTo(Drive *iDrive,
         const double iTrackerTrust,
         std::shared_ptr<PointAt> iPointAt,
         std::unique_ptr<LateralProfileFollower> iFollower,
         std::unique_ptr<Controller> iStraight = nullptr,
         const inch_t iDistanceToSwitch = 0.0_in);

  void go(const Position &target,
          const bool reversed = false,
          const std::optional<LateralMotionProfile::MotionConstraints>
              &specialConstraints = {});

  private:
  meters_per_second_t getVelocity() const;

  double getStraighteningOutput(const Position &initial,
                                const Position &to,
                                const bool reversed);

  Drive *drive;
  const double trackerTrust;
  std::shared_ptr<PointAt> pointAt;
  std::unique_ptr<LateralProfileFollower> follower;
  std::unique_ptr<Controller> straight;
  const inch_t distanceToSwitch;
};
} // namespace atum