#include "trajectory.hpp"

namespace atum {
Trajectory::Trajectory(const std::string &name,
                       std::vector<Position> waypoints,
                       const double curvature,
                       const Constraints &constraints,
                       const meter_t driveWidth,
                       const bool update) {
  const std::string filename{dir + name + ".csv"};
  if(fileExists(filename) && !update) {
    std::fstream trajFile{filename, std::fstream::in};
    points = squiggles::deserialize_path(trajFile).value();
  } else {
    std::fstream trajFile{filename, std::fstream::out};
    squiggles::Constraints squigglesConstraints{
        getValueAs<meters_per_second_t>(constraints.maxV),
        getValueAs<meters_per_second_squared_t>(constraints.maxA),
        getValueAs<meters_per_second_cubed_t>(constraints.maxJ)};
    squigglesConstraints.max_curvature *= curvature;
    const std::vector<squiggles::ControlVector> squigglesWaypoints{
        toSquigglesWaypoints(waypoints, squigglesConstraints)};
    squiggles::SplineGenerator generator{
        squigglesConstraints,
        std::make_shared<squiggles::TankModel>(getValueAs<meter_t>(driveWidth),
                                               squigglesConstraints),
        0.01};
    points = generator.generate(squigglesWaypoints);
    squiggles::serialize_path(trajFile, points);
  }
}

std::optional<RawPosition> Trajectory::next() {
  if(index >= points.size()) return {};
  return toRawPosition(points[index++]);
}

void Trajectory::resetAllTrajs() {
  for(const auto &entry : std::filesystem::directory_iterator(dir))
    std::filesystem::remove_all(entry.path());
}

RawPosition
    Trajectory::toRawPosition(const squiggles::ProfilePoint &squigglesPoint) {
  return RawPosition{squigglesPoint.vector.pose.x,
                     squigglesPoint.vector.pose.y,
                     M_PI_2 - squigglesPoint.vector.pose.yaw,
                     squigglesPoint.vector.vel,
                     -squigglesPoint.curvature * squigglesPoint.vector.vel};
}

const std::string Trajectory::dir{"/usd/trajs/"};

std::vector<squiggles::ControlVector> Trajectory::toSquigglesWaypoints(
    const std::vector<Position> &waypoints,
    const squiggles::Constraints &squigglesConstraints) const {
  std::vector<squiggles::ControlVector> squigglesWaypoints;
  for(RawPosition waypoint : waypoints) {
    if(!waypoint.v) waypoint.v = squigglesConstraints.max_vel;
    squigglesWaypoints.push_back(
        {{waypoint.x, waypoint.y, M_PI_2 - waypoint.h}, waypoint.v});
  }
  if(!waypoints[0].v) squigglesWaypoints[0].vel = 0;
  if(!waypoints.back().v) squigglesWaypoints.back().vel = 0;
  return squigglesWaypoints;
}
} // namespace atum