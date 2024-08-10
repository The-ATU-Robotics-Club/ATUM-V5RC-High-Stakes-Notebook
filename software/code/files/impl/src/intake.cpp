#include "intake.hpp"

namespace atum {
Intake::Intake(std::unique_ptr<pros::Motor> iRoller,
               std::unique_ptr<pros::Motor> iArmL,
               std::unique_ptr<pros::Motor> iArmR,
               std::unique_ptr<pros::Distance> iPreload1Eye,
               std::unique_ptr<pros::Distance> iPreload2Eye,
               std::unique_ptr<pros::Distance> iGrabEye,
               std::unique_ptr<Potentiometer> iPotL,
               std::unique_ptr<Potentiometer> iPotR,
               std::unique_ptr<AngularProfileFollower> iRetractFollower,
               std::unique_ptr<AngularProfileFollower> iExtendFollower,
               std::unique_ptr<Controller> iBalance,
               std::unique_ptr<Controller> iHold,
               const Parameters &iParams,
               std::unique_ptr<Logger> iLogger) :
    Task(Task::TaskParams{"Intake Macros"},
         Task::TaskParams{"Intake Maintenance"}),
    roller{std::move(iRoller)},
    armL{std::move(iArmL)},
    armR{std::move(iArmR)},
    preload1Eye{std::move(iPreload1Eye)},
    preload2Eye{std::move(iPreload2Eye)},
    grabEye{std::move(iGrabEye)},
    potL{std::move(iPotL)},
    potR{std::move(iPotR)},
    retractFollower{std::move(iRetractFollower)},
    extendFollower{std::move(iExtendFollower)},
    balance{std::move(iBalance)},
    hold{std::move(iHold)},
    params{iParams},
    logger{std::move(iLogger)} {
  start();
  if(logger) logger->info("Intake constructed!");
  armL->tare_position();
  armR->tare_position();
}

void Intake::removeBall(const second_t removeTime) {
  outtake();
  wait(removeTime);
  stopRoller();
}

void Intake::runRoller(const double voltage) {
  roller->move_voltage(voltage);
}

void Intake::stopRoller() {
  roller->move_voltage(0);
}

void Intake::intake() {
  runRoller(params.intakeInVoltage);
}

void Intake::outtake() {
  runRoller(params.intakeOutVoltage);
}

void Intake::runArm(double voltage) {
  armVoltage = voltage;
}

void Intake::stopArm() {
  runArm(0);
}

void Intake::manualExtend() {
  test.slew(params.armManualExtendVoltage);
  runArm(params.armManualExtendVoltage);
}

void Intake::manualRetract() {
  test.slew(params.armManualRetractVoltage);
  runArm(params.armManualRetractVoltage);
}

void Intake::manualStopArm() {
  test.slew(0);
  runArm(test.getOutput());
}

void Intake::extend() {
  desired = params.extendedPosition;
}

void Intake::retract() {
  desired = 0_deg;
}

void Intake::armTo(const degree_t iDesired) {
  desired = iDesired;
}

void Intake::stopMacros() {
  macroState = MacroState::NA;
  holdState = HoldState::AllowRetract;
}

void Intake::preload() {
  macroState = MacroState::Preloading;
}

bool Intake::isPreloading() const {
  return macroState == MacroState::Preloading;
}

void Intake::grab() {
  macroState = MacroState::Grabbing;
}

bool Intake::isGrabbing() const {
  return macroState == MacroState::Grabbing;
}

void Intake::setHold(const HoldState &iHoldState) {
  holdState = iHoldState;
}

bool Intake::preloadDetected() const {
  const millimeter_t distance1{preload1Eye->get()};
  // Account for having one or two distance sensors.
  millimeter_t distance2{distance1};
  if(preload2Eye) distance2 = millimeter_t{preload2Eye->get()};
  return distance1 <= params.preloadDistance ||
         distance2 <= params.preloadDistance;
}

bool Intake::grabDetected() const {
  const millimeter_t distance{grabEye->get()};
  return distance <= params.grabDistance;
}

void Intake::taskFn1() {
  while(true) {
    switch(macroState) {
      case MacroState::Preloading: preloadMacro(); break;
      case MacroState::Grabbing: grabMacro(); break;
      default:
        if(desired) blockingRunArm(desired.value(), MacroState::NA);
        break;
    }
    wait(10_ms);
  }
}

void Intake::taskFn2() {
  while(true) {
    if(logger) logger->debug("Arm: " + to_string(getArmPosition()));
    double readingArmL{armL->get_position()};
    double readingArmR{armR->get_position()};
    double balanceOutput{0.0};
    if(readingArmL != PROS_ERR_F && readingArmR != PROS_ERR_F)
      balanceOutput =
          balance->getOutput(armL->get_position(), armR->get_position());
    double output{0.0};
    const degree_t currentPosition{getArmPosition()};
    if(!armVoltage) {
      switch(holdState) {
        case HoldState::AllowRetract:
          holdPosition = std::min(holdPosition, currentPosition);
          break;
        case HoldState::AllowExtend:
          holdPosition = std::max(holdPosition, currentPosition);
          break;
        default: holdPosition = currentPosition; break;
      }
      if(holdState != HoldState::NA)
        output += hold->getOutput(getValueAs<degree_t>(currentPosition),
                                  getValueAs<degree_t>(holdPosition));
    } else
      holdPosition = currentPosition;
    output += armVoltage;
    armL->move_voltage(output + balanceOutput);
    armR->move_voltage(output - balanceOutput);
    wait(10_ms);
  }
}

void Intake::preloadMacro() {
  intake();
  blockingRunArm(params.loadingPosition, MacroState::Preloading);
  waitUntil(
      [this]() { return !this->isPreloading() || this->preloadDetected(); },
      params.preloadTimeout);
  waitUntil([this]() { return !this->isPreloading(); }, params.preloadDelay);
  blockingRunArm(params.extendedPosition, MacroState::Preloading);
  if(params.intakeDelay)
    waitUntil([this]() { return !this->isPreloading(); }, params.intakeDelay);
  blockingRunArm(params.shootingPosition, MacroState::Preloading);
  if(params.shootTime)
    waitUntil([this]() { return !this->isPreloading(); }, params.shootTime);
  macroState = MacroState::NA;
}

void Intake::grabMacro() {
  holdState = HoldState::AllowExtend;
  blockingRunArm(params.loadingPosition, MacroState::Grabbing);
  intake();
  blockingRunArm(params.extendedPosition, MacroState::Grabbing);
  waitUntil([this]() { return !this->isGrabbing() || this->grabDetected(); });
  stopRoller();
  holdState = HoldState::AllowRetract;
  macroState = MacroState::NA;
}

void Intake::blockingRunArm(degree_t target, const MacroState whileState) {
  const degree_t initialPosition = getArmPosition();
  target = constrain180(target - initialPosition);
  AngularProfileFollower *follower = extendFollower.get();
  if(target < 0_deg) follower = retractFollower.get();
  follower->reset(target);
  std::optional<double> output{follower->getOutput(
      constrain180(getArmPosition() - initialPosition), getArmVelocity())};
  while(output && macroState == whileState) {
    output = follower->getOutput(
        constrain180(getArmPosition() - initialPosition), getArmVelocity());
    runArm(output.value_or(0.0));
    wait(10_ms);
  }
  stopArm();
  desired = {};
}

degree_t Intake::getArmPosition() const {
  const degree_t readingPotL{potL->getPosition()};
  const degree_t readingPotR{potR->getPosition()};
  double readingArmL{armL->get_position()};
  if(readingArmL == PROS_ERR_F) readingArmL = getValueAs<degree_t>(readingPotL);
  double readingArmR{armR->get_position()};
  if(readingArmR == PROS_ERR_F) readingArmR = getValueAs<degree_t>(readingPotR);
  return (degree_t{readingArmL} + readingPotL + degree_t{readingArmR} +
          readingPotR) /
         4.0;
}

degrees_per_second_t Intake::getArmVelocity() {
  const revolutions_per_minute_t readingPotL{potL->getVelocity()};
  const revolutions_per_minute_t readingPotR{potR->getVelocity()};
  double readingArmL{armL->get_actual_velocity()};
  if(readingArmL == PROS_ERR_F)
    readingArmL = getValueAs<revolutions_per_minute_t>(readingPotL);
  double readingArmR{armR->get_actual_velocity()};
  if(readingArmR == PROS_ERR_F)
    readingArmR = getValueAs<revolutions_per_minute_t>(readingPotR);
  return (revolutions_per_minute_t{readingArmL} + readingPotL +
          revolutions_per_minute_t{readingArmR} + readingPotR) /
         4.0;
}

void Intake::setBrakeMode(const pros::motor_brake_mode_e_t brakemode) {
  armL->set_brake_mode(brakemode);
  armR->set_brake_mode(brakemode);
}
} // namespace atum