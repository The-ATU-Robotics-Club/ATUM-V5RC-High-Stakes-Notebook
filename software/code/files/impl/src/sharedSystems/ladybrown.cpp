#include "ladybrown.hpp"

namespace atum {
Ladybrown::Ladybrown(std::unique_ptr<Motor> iLeft,
                     std::unique_ptr<Motor> iRight,
                     std::unique_ptr<Piston> iPiston,
                     std::unique_ptr<RotationSensor> iRotation,
                     std::unique_ptr<LineTracker> iLine,
                     const Parameters &iParams,
                     std::unique_ptr<AngularProfileFollower> iFollower,
                     const Logger::Level loggerLevel) :
    Task{this, loggerLevel},
    left{std::move(iLeft)},
    right{std::move(iRight)},
    piston{std::move(iPiston)},
    rotation{std::move(iRotation)},
    line{std::move(iLine)},
    params{iParams},
    follower{std::move(iFollower)},
    logger{loggerLevel} {
  // Reset devices.
  left->setBrakeMode(pros::MotorBrake::brake);
  right->setBrakeMode(pros::MotorBrake::brake);
  left->resetPosition(params.statePositions[LadybrownState::Resting].value());
  right->resetPosition(params.statePositions[LadybrownState::Resting].value());
  rotation->resetDisplacement(
      params.statePositions[LadybrownState::Resting].value());

  // Manage stored positions and state.
  params.statePositions[LadybrownState::Extending] = {};
  params.statePositions[LadybrownState::Retracting] = {};
  params.statePositions[LadybrownState::FinishScoring] =
      params.statePositions[LadybrownState::Preparing];
  params.statePositions[LadybrownState::FullyExtending] =
      params.statePositions[LadybrownState::Scoring];
  stop();

  logger.info("Ladybrown is constructed!");
}

void Ladybrown::stop() {
  state = LadybrownState::Idle;
  // During manual control, hold wherever you stop.
  if(!holdPosition.has_value() && getVelocity() == 0_rpm) {
    holdPosition = getPosition();
  }
}

void Ladybrown::extend() {
  changeState(LadybrownState::Extending, true);
}

void Ladybrown::retract() {
  changeState(LadybrownState::Retracting, true);
}

void Ladybrown::rest() {
  changeState(LadybrownState::Resting, false);
}

void Ladybrown::load() {
  changeState(LadybrownState::Loading, false);
}

void Ladybrown::prepare() {
  changeState(LadybrownState::Preparing, false);
}

void Ladybrown::score() {
  // Don't override back up on the arm after scoring.
  if(state == LadybrownState::FinishScoring) {
    return;
  }
  changeState(LadybrownState::Scoring, false);
}

void Ladybrown::finishScore() {
  // Don't bother with motion profiled movements if nearby.
  if(getClosestNamedPosition() == LadybrownState::Preparing) {
    return;
  }
  changeState(LadybrownState::FinishScoring, false);
}

void Ladybrown::fullyExtend() {
  // Don't bother with motion profiled movements if nearby.
  if(getClosestNamedPosition() == LadybrownState::Scoring) {
    return;
  }
  changeState(LadybrownState::FullyExtending, false);
}

LadybrownState Ladybrown::getClosestNamedPosition() const {
  // Don't give a measurement if you are still moving.
  if(state != LadybrownState::Idle) {
    return LadybrownState::Idle;
  }
  degree_t shortestDistance{infinite};
  LadybrownState closestPosition{LadybrownState::Resting};
  for(const auto &[position, angle] : params.statePositions) {
    if(!angle.has_value()) {
      continue;
    }
    const degree_t distance{abs(angle.value() - getPosition())};
    if(distance <= shortestDistance) {
      shortestDistance = distance;
      closestPosition = position;
    }
  }
  return closestPosition;
}

bool Ladybrown::hasRing() const {
  return getClosestNamedPosition() != LadybrownState::Resting &&
         line->check() && line->triggered();
}

bool Ladybrown::readyToScore() {
  return getClosestNamedPosition() != LadybrownState::Loading &&
         (hasRing() || noRingDetection());
}

bool Ladybrown::noRingDetection() {
  return !line->check();
}

bool Ladybrown::mayConflictWithIntake() {
  return getClosestNamedPosition() == LadybrownState::Loading && hasRing();
}

void Ladybrown::changeState(const LadybrownState newState,
                            const bool iEnableSlew) {
  // Don't bother with motion profiled movements if nearby.
  if(getClosestNamedPosition() == newState) {
    return;
  }
  enableSlew = iEnableSlew;
  state = newState;
  holdPosition = params.statePositions[state];
}

void Ladybrown::moveTo(const LadybrownState targetState) {
  const degree_t target{params.statePositions[targetState].value()};
  const LadybrownState startingState{state};
  follower->startProfile(getPosition(), target);
  while(!follower->isDone() && state == startingState) {
    const double followerOutput{
        follower->getOutput(getPosition(), getVelocity())};
    voltage = followerOutput;
    wait();
  }
  stop();
}

degree_t Ladybrown::getPosition() const {
  if(rotation->check()) {
    // Just use reading from rotation sensor if available.
    return rotation->getDisplacement();
  }
  std::vector<degree_t> readings;
  if(left->check()) {
    readings.push_back(left->getPosition());
  }
  if(right->check()) {
    readings.push_back(right->getPosition());
  }
  return average(readings);
}

degrees_per_second_t Ladybrown::getVelocity() const {
  std::vector<degrees_per_second_t> readings;
  if(rotation->check()) {
    readings.push_back(rotation->getVelocity());
  }
  if(left->check()) {
    readings.push_back(left->getVelocity());
  }
  if(right->check()) {
    readings.push_back(right->getVelocity());
  }
  return average(readings);
}

void Ladybrown::handlePiston() {
  if(getPosition() >= params.flippingPosition) {
    piston->extend();
  } else {
    if(piston->isExtended()) {
      piston->retract();
      left->brake();
      right->brake();
      wait(params.pistonDelay);
    }
  }
}

bool Ladybrown::maintainMotors() {
  // If one is still working, reset the position of the others so everything
  // works if it comes back online.
  if(!left->check()) {
    left->resetPosition(getPosition());
  }
  if(!right->check()) {
    right->resetPosition(getPosition());
  }

  if(getPosition() <= params.noMovePosition && voltage <= 0) {
    left->brake();
    right->brake();
    return true;
  }
  return false;
}

double Ladybrown::getHoldOutput() {
  if(getPosition() < params.noMovePosition) {
    return 0.0;
  }
  double holdOutput{0.0};
  if(holdPosition.has_value() && state == LadybrownState::Idle) {
    const double holdError{
        getValueAs<degree_t>(holdPosition.value() - getPosition())};
    const double hold{params.holdController.getOutput(holdError)};
    holdOutput += hold;
  }
  holdOutput += params.kG * cos(getValueAs<radian_t>(getPosition()));
  return holdOutput;
}

TASK_DEFINITIONS_FOR(Ladybrown) {
  START_TASK("Ladybrown State Machine")
  while(true) {
    switch(state) {
      case LadybrownState::Idle: voltage = 0; break;
      case LadybrownState::Extending: voltage = params.manualVoltage; break;
      case LadybrownState::Retracting: voltage = -params.manualVoltage; break;
      default: moveTo(state); break;
    }
    wait();
  }
  END_TASK

  START_TASK("Ladybrown Control")
  while(true) {
    wait(); // At the top because of continue statement below.
    handlePiston();
    if(maintainMotors()) {
      continue;
    }
    double output{0.0};
    params.manualSlew.slew(voltage);
    if(enableSlew) {
      output += params.manualSlew.getOutput();
    } else {
      output = voltage;
    }
    output += getHoldOutput();
    double balance{0.0};
    if(getValueAs<degree_t>(getPosition()) >= params.noMovePosition() &&
       left->check() && right->check()) {
      balance = params.balanceController.getOutput(
          getValueAs<degree_t>(left->getPosition()),
          getValueAs<degree_t>(right->getPosition()));
    }
    left->moveVoltage(output + balance);
    right->moveVoltage(output);
  }
  END_TASK
} // namespace atum
} // namespace atum