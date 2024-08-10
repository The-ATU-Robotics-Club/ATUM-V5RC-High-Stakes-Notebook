#pragma once

#include "../depend/squiggles/squiggles.hpp"
#include "../position/position.hpp"
#include "motionProfile.hpp"
#include <filesystem>

namespace atum {
class Trajectory {
  public:
  using Constraints = LateralMotionProfile::MotionConstraints;

  Trajectory(const std::string &name,
             std::vector<Position> waypoints,
             const double curvature,
             const Constraints &constraints,
             const meter_t driveWidth,
             const bool update);

  std::optional<RawPosition> next();

  static void resetAllTrajs();

  static RawPosition
      toRawPosition(const squiggles::ProfilePoint &squigglesPoint);

  private:
  std::vector<squiggles::ControlVector> toSquigglesWaypoints(
      const std::vector<Position> &waypoints,
      const squiggles::Constraints &squigglesConstraints) const;

  static const std::string dir;

  std::vector<squiggles::ProfilePoint> points;
  std::size_t index{0};
};

} // namespace atum