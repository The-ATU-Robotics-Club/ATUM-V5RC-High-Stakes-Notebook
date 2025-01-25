#include "pathFollower.hpp"

namespace atum {
PathFollower::Command::Command(const Pose &iTarget,
                               bool iReversed,
                               std::optional<Path::Parameters> iParams,
                               std::optional<AcceptableDistance> iAcceptable) :
    target{iTarget},
    reversed{iReversed},
    params{iParams},
    acceptable{iAcceptable} {}

PathFollower::PathFollower(Drive *iDrive,
                           const AcceptableDistance &iDefaultAcceptable,
                           std::unique_ptr<Controller> iLeft,
                           std::unique_ptr<Controller> iRight,
                           const AccelerationConstants &iKA,
                           const FeedbackParameters &iFeedbackParams,
                           const Logger::Level loggerLevel) :
    drive{iDrive},
    defaultAcceptable{iDefaultAcceptable},
    left{std::move(iLeft)},
    right{std::move(iRight)},
    kA{iKA},
    feedbackParams{iFeedbackParams},
    logger{loggerLevel} {
  prepareGraph();
}

void PathFollower::follow(const std::vector<Command> &commands,
                          const std::string &name) {
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
  Pose start{drive->getPose()};
  if(flipped) {
    cmd.target.flip();
  }
  if(cmd.reversed) {
    start.h += 180_deg;
    cmd.target.h += 180_deg;
  }
  Path traj{{start, cmd.target}, cmd.params, logger.getLevel()};
  Acceptable acceptable{cmd.acceptable.value_or(defaultAcceptable)};
  acceptable.reset(cmd.timeoutScaling * traj.getTotalTime());
  left->reset();
  right->reset();
  while(traj.getPose(drive->getPose()) != cmd.target &&
        !acceptable.canAccept(distance(drive->getPose(), cmd.target)) &&
        !interrupted) {
    UnwrappedPose state{drive->getPose()};
    if(cmd.reversed) {
      state.h += M_PI;
    }
    UnwrappedPose target{traj.getPose(drive->getPose())};
    auto [refVL, refVR] = getLRReference(state, target, cmd.reversed);
    auto [vL, vR] = drive->getLRVelocity();
    const double stateVL{getValueAs<revolutions_per_minute_t>(vL)};
    const double stateVR{getValueAs<revolutions_per_minute_t>(vR)};
    const double aFF = getAccelFeedforward(target.a, cmd.reversed);
    drive->tank(left->getOutput(stateVL, refVL) + aFF,
                right->getOutput(stateVR, refVR) + aFF);
    graphPoints(stateVL, refVL, stateVR, refVR);
    wait();
  }
}

void PathFollower::interrupt() {
  interrupted = true;
}

void PathFollower::setFlipped(const bool iFlipped) {
  flipped = iFlipped;
}

std::pair<double, double>
    PathFollower::getLRReference(const UnwrappedPose &state,
                                 const UnwrappedPose &target,
                                 const bool reversed) {
  const auto [refV, refOmega] = getReference(state, target);
  const auto [refVL, refVR] = toRPM(refV, refOmega);
  if(reversed) {
    return {-refVR, -refVL};
  }
  return {refVL, refVR};
}

std::pair<double, double>
    PathFollower::getReference(const UnwrappedPose &state,
                               const UnwrappedPose &target) {
  if(!feedbackParams.useRAMSETE) {
    return {target.v, target.omega};
  }
  const UnwrappedPose error{getError(state, target)};
  const double k{
      2.0 * feedbackParams.lambda *
      sqrt(pow(target.omega, 2.0) + feedbackParams.beta * pow(target.v, 2.0))};
  const double v{target.v * cos(error.h) + k * error.x};
  const double sincHError{error.h ? sin(error.h) / error.h : 1.0};
  const double omega{target.omega + k * error.h +
                     feedbackParams.beta * target.v * sincHError * error.y};
  return {v, omega};
}

UnwrappedPose PathFollower::getError(const UnwrappedPose &state,
                                     const UnwrappedPose &target) {
  UnwrappedPose error{target - state};
  const double h{M_PI_2 - state.h};
  const double globalXError{error.x * cos(h) + error.y * sin(h)};
  const double globalYError{error.x * -sin(h) + error.y * cos(h)};
  error.x = globalXError;
  error.y = globalYError;
  error.h = constrainPI(target.h - state.h);
  return error;
}

std::pair<double, double> PathFollower::toRPM(const double v,
                                              const double omega) {
  auto [leftV, rightV] = toLR(v, omega);
  const double maxV{getValueAs<meters_per_second_t>(drive->getMaxVelocity())};
  const double scalar{max(std::abs(leftV), std::abs(rightV)) / maxV};
  if(scalar > 1.0) {
    leftV /= scalar;
    rightV /= scalar;
  }
  const double mpsToRPM{60.0 / drive->getGeometry().circum};
  return {mpsToRPM * leftV, mpsToRPM * rightV};
}

double PathFollower::getAccelFeedforward(const double a, const bool reversed) {
  double coeff{a > 0.0 ? kA.accel : kA.decel};
  if(reversed) {
    coeff *= -1;
  }
  return coeff * a;
}

std::pair<double, double> PathFollower::toLR(const double lateral,
                                             const double angular) {
  const Drive::Geometry geometry{drive->getGeometry()};
  const double track{getValueAs<meter_t>(geometry.track)};
  double angularAdjustment{angular * (track / 2.0)};
  double leftV{lateral + angularAdjustment};
  double rightV{lateral - angularAdjustment};
  return {leftV, rightV};
}

void PathFollower::prepareGraph() {
  if(logger.getLevel() != Logger::Level::Debug) {
    return;
  }
  GUI::Graph::clearSeries(GUI::SeriesColor::Magenta);
  GUI::Graph::clearSeries(GUI::SeriesColor::Cyan);
  const double maxV{getValueAs<revolutions_per_minute_t>(drive->getMaxRPM())};
  GUI::Graph::setSeriesRange(maxV, GUI::SeriesColor::Red);
  GUI::Graph::setSeriesRange(maxV, GUI::SeriesColor::Magenta);
  GUI::Graph::setSeriesRange(maxV, GUI::SeriesColor::Blue);
  GUI::Graph::setSeriesRange(maxV, GUI::SeriesColor::Cyan);
}

void PathFollower::graphPoints(const double stateVL,
                               const double refVL,
                               const double stateVR,
                               const double refVR) {
  if(logger.getLevel() != Logger::Level::Debug) {
    return;
  }
  GUI::Graph::addValue(stateVL, GUI::SeriesColor::Red);
  GUI::Graph::addValue(refVL, GUI::SeriesColor::Magenta);
  GUI::Graph::addValue(stateVR, GUI::SeriesColor::Blue);
  GUI::Graph::addValue(refVR, GUI::SeriesColor::Cyan);
}
} // namespace atum