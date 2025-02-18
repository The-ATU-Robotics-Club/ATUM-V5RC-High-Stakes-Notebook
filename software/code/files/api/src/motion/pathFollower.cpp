#include "pathFollower.hpp"
#include "atum/depend/units.h"
#include "atum/gui/screen.hpp"
#include "atum/pose/pose.hpp"
#include "atum/utility/units.hpp"
#include "path.hpp"


namespace atum {
PathFollower::Command::Command(std::optional<AcceptableDistance> iAcceptable,
                               const Pose &iStart,
                               const Pose &iTarget,
                               const bool iReversed,
                               std::optional<Path::Parameters> iParams) :
    acceptable{iAcceptable},
    start{iStart},
    target{iTarget},
    reversed{iReversed},
    params{iParams} {}

PathFollower::Command::Command(std::optional<AcceptableDistance> iAcceptable,
                               const Pose &iTarget,
                               const bool iReversed,
                               std::optional<Path::Parameters> iParams) :
    acceptable{iAcceptable},
    target{iTarget},
    reversed{iReversed},
    params{iParams} {}

PathFollower::PathFollower(Drive *iDrive,
                           const AcceptableDistance &iDefaultAcceptable,
                           std::unique_ptr<Controller> iForward,
                           std::unique_ptr<Controller> iTurn,
                           const AccelerationConstants &iKA,
                           const meter_t iLookaheadDistance,
                           const Logger::Level loggerLevel) :
    drive{iDrive},
    defaultAcceptable{iDefaultAcceptable},
    forward{std::move(iForward)},
    turn{std::move(iTurn)},
    kA{iKA},
    lookaheadDistance{getValueAs<meter_t>(iLookaheadDistance)},
    logger{loggerLevel} {
  prepareGraph();
}

void PathFollower::follow(const std::vector<Command> &commands,
                          const std::string &name) {
  interrupted = false;
  if(name.empty()) {
    logger.debug("Following a path.");
  } else {
    logger.debug("Following a path, \"" + name + ".\"");
  }
  for(int i{0}; i < commands.size() && !interrupted; i++) {
    follow(commands[i]);
  }
  drive->brake();
  if(interrupted) {
    logger.debug("Path following was interrupted!");
    interrupted = false;
  } else {
    logger.debug("Path following complete!");
  }
}

void PathFollower::follow(Command cmd) {
  reset(cmd);
  Acceptable acceptable{cmd.acceptable.value_or(defaultAcceptable)};
  UnwrappedPose state{drive->getPose()};
  while(getClosest(state) != path->getPose(path->getSize() - 1) &&
        !acceptable.canAccept(distance(drive->getPose(), cmd.target)) &&
        !interrupted) {
    state = drive->getPose();
    auto [refV, refH] = getVHReference(state);
    const double aFF = getAccelFeedforward(refV, cmd.reversed);
    if(cmd.reversed) {
      refV *= -1.0;
      refH += M_PI;
    }
    double hError{constrainPI(refH - state.h)};
    refV *= std::abs(std::cos(hError));
    const meters_per_second_t v{
        std::abs(getValueAs<meters_per_second_t>(drive->getVelocity()))};
    const double forwardOutput{forward->getOutput(state.v, refV)};
    const double turnOutput{turn->getOutput(hError)};
    drive->tank(forwardOutput + turnOutput + aFF,
                forwardOutput - turnOutput + aFF);
    graphPoints(state.v, refV);
    wait();
  }
}

void PathFollower::reset(PathFollower::Command &cmd) {
  if(flipped) {
    if(cmd.start.has_value()) {
      cmd.start.value().flip();
    }
    cmd.target.flip();
  }
  Pose start{cmd.start.value_or(drive->getPose())};
  if(cmd.reversed) {
    start.h += 180_deg;
    cmd.target.h += 180_deg;
  }
  path = std::make_unique<Path>(
      std::make_pair(start, cmd.target), cmd.params, logger.getLevel());
  const double maxVelChange{
      getValueAs<meters_per_second_t>(path->getParams().maxA * standardDelay)};
  accelLimiter =
      std::make_unique<SlewRate>(std::make_pair(infinite, maxVelChange),
                                 getValueAs<meters_per_second_t>(start.v));
  forward->reset();
  turn->reset();
  closestIndex = 0;
  lookahead = path->getPose(0);
  lookaheadIndex = 0;
  prevRefV = 0.0;
}

std::pair<double, double> PathFollower::getVHReference(const Pose &state) {
  radian_t lookaheadAngle;
  if(lookaheadIndex / static_cast<double>(path->getSize()) >= 0.90) {
    lookaheadAngle = path->getPose(path->getSize() - 1).h;
  } else {
    lookaheadAngle = angle(state, getLookahead(state));
  }
  return {getValueAs<meters_per_second_t>(closest.v),
          getValueAs<radian_t>(lookaheadAngle)};
}

double PathFollower::getAccelFeedforward(const double refV,
                                         const bool reversed) {
  const double dt{getValueAs<second_t>(standardDelay)};
  const double a{(refV - prevRefV) / dt};
  prevRefV = refV;
  double coeff{a > 0.0 ? kA.accel : kA.decel};
  if(reversed) {
    coeff *= -1.0;
  }
  return coeff * a;
}

Pose PathFollower::getLookahead(const Pose &state) {
  const double lookaheadProximity{lookaheadDistance * lookaheadDistance};
  for(int i{lookaheadIndex}; i < path->getSize() - 1; i++) {
    const double firstProximity{proximity(state, path->getPose(i))};
    const double firstDiff{lookaheadProximity - firstProximity};
    const double secondProximity{proximity(state, path->getPose(i + 1))};
    const double secondDiff{lookaheadProximity - secondProximity};
    // If one is within the lookahead sphere and the other isn't.
    if(firstDiff * secondDiff < 0.0) {
      lookaheadIndex = i + 1;
      lookahead = path->getPose(lookaheadIndex);
      break;
    }
  }
  return lookahead;
}

Pose PathFollower::getClosest(const Pose &state) {
  double nearestProximity{proximity(state, path->getPose(closestIndex))};
  double lastProximity{nearestProximity};
  for(int i{closestIndex}; i < path->getSize(); i++) {
    const double maybeCloserProximity{proximity(state, path->getPose(i))};
    if(maybeCloserProximity < nearestProximity) {
      nearestProximity = maybeCloserProximity;
      closestIndex = i;
    } else if(maybeCloserProximity > lastProximity) {
      break;
    }
    lastProximity = maybeCloserProximity;
  }
  Pose closestPose{path->getPose(closestIndex)};
  const meters_per_second_t adjV{
      accelLimiter->slew(getValueAs<meters_per_second_t>(closestPose.v))};
  closestPose.v = adjV;
  closest = closestPose;
  return closestPose;
}

double PathFollower::proximity(const UnwrappedPose &p0,
                               const UnwrappedPose &p1) const {
  const double dx{p1.x - p0.x};
  const double dy{p1.y - p0.y};
  return dx * dx + dy * dy;
}

void PathFollower::prepareGraph() {
  if(logger.getLevel() != Logger::Level::Debug) {
    return;
  }
  GUI::Graph::clearSeries(GUI::SeriesColor::Red);
  GUI::Graph::clearSeries(GUI::SeriesColor::Magenta);
  const double maxV{getValueAs<meters_per_second_t>(drive->getMaxVelocity())};
  GUI::Graph::setSeriesRange(maxV, GUI::SeriesColor::Red);
  GUI::Graph::setSeriesRange(maxV, GUI::SeriesColor::Magenta);
}

void PathFollower::graphPoints(const double stateV, const double refV) {
  if(logger.getLevel() != Logger::Level::Debug) {
    return;
  }
  GUI::Graph::addValue(stateV, GUI::SeriesColor::Red);
  GUI::Graph::addValue(refV, GUI::SeriesColor::Magenta);
  GUI::Map::addPosition(path->getPose(closestIndex), GUI::SeriesColor::Blue);
  GUI::Map::addPosition(lookahead, GUI::SeriesColor::White);
}
} // namespace atum