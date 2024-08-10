#include "ramsete.hpp"

namespace atum {
RAMSETE::RAMSETE(Drive *iDrive,
                 std::shared_ptr<PointAt> iPointAt,
                 std::unique_ptr<Controller> iLeft,
                 std::unique_ptr<Controller> iRight,
                 const Constraints &iDefaultConstraints,
                 const double iBeta,
                 const double iLambda) :
    drive{iDrive},
    pointAt{iPointAt},
    left{std::move(iLeft)},
    right{std::move(iRight)},
    defaultConstraints{iDefaultConstraints},
    beta{iBeta},
    lambda{iLambda} {}

void RAMSETE::go(const std::string &trajName,
                 const bool updateTraj,
                 std::vector<Position> waypoints,
                 const bool reversed,
                 const std::optional<Constraints> &specialConstraints,
                 const double curvature) {
  pointAt->go(waypoints[0].h, reversed);
  left->reset();
  right->reset();
  Constraints constraints = defaultConstraints;
  if(specialConstraints) {
    if(specialConstraints.value().maxV != 0.0_in_per_s)
      constraints.maxV = specialConstraints.value().maxV;
    if(specialConstraints.value().maxA != 0.0_in_per_s_sq)
      constraints.maxA = specialConstraints.value().maxA;
    if(specialConstraints.value().maxJ != 0.0_in_per_s_cb)
      constraints.maxJ = specialConstraints.value().maxJ;
  }
  Trajectory trajectory{trajName,
                        waypoints,
                        curvature,
                        constraints,
                        drive->getGeometry().driveWidth,
                        updateTraj};
  while(const std::optional<RawPosition> point = trajectory.next()) {
    performRAMSETE(RawPosition{drive->getPosition()}, point.value(), reversed);
    wait(10_ms);
  }
  drive->tank(0, 0);
}

void RAMSETE::performRAMSETE(RawPosition state,
                             const RawPosition &target,
                             const bool reversed) {
  GUI::graph("Drive RPM", std::make_pair(-600, 600), 1000);
  if(reversed) state.h += M_PI;
  const RawPosition error{getError(state, target)};
  const double k{2.0 * lambda *
                 sqrt(pow(target.w, 2.0) + beta * pow(target.v, 2.0))};
  const double v{target.v * cos(error.h) + k * error.x};
  const double sincHError{error.h ? sin(error.h) / error.h : 1.0};
  const double w{target.w + k * error.h +
                 beta * target.v * sincHError * error.y};
  const auto [refVL, refVR] = drive->toLRVelocity(v, w);
  const auto [stateVL, stateVR] = drive->getLRVelocity();
  GUI::greenPlot(abs(refVR));
  GUI::redPlot(abs(stateVR));
  if(reversed)
    drive->tank(left->getOutput(stateVL, -refVR),
                right->getOutput(stateVR, -refVL));
  else
    drive->tank(left->getOutput(stateVL, refVL),
                right->getOutput(stateVR, refVR));
}

RawPosition RAMSETE::getError(const RawPosition &state,
                              const RawPosition &target) {
  RawPosition error{target - state};
  const double globalXError{error.x * sin(state.h) + error.y * cos(state.h)};
  const double globalYError{error.x * cos(state.h) - error.y * sin(state.h)};
  error.x = globalXError;
  error.y = globalYError;
  return error;
}
} // namespace atum