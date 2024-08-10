#include "robot24In.hpp"

namespace atum {

void Robot24In::opcontrol() {
  if(logger) logger->info("Operator control has started!");
  schedule(ScheduledAction{[]() { return false; },
                           [this]() { this->remote.rumble("---"); },
                           "Ten Seconds Left",
                           65_s});
  intake->stopMacros();
  if(GUI::getRoutine() == Routine::Skills)
    flywheel->setSpeed(2);
  else
    flywheel->setSpeed(0);
  drive->setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
  while(true) {
    wait(10_ms);
    opcontrolRemotePrint();
    driveControls();
    // If using endgame, don't use intake or wings.
    if(endGameControls()) continue;
    intakeControls();
    flywheelControls();
    wingControls();
  }
}

void Robot24In::opcontrolRemotePrint() {
  remote.print(0,
               "Flywheel: " +
                   std::to_string(static_cast<int>(flywheel->getReference())) +
                   " RPM");
  remote.print(
      1,
      "Brain: " +
          std::to_string(static_cast<int>(pros::c::battery_get_capacity())) +
          "%");
  remote.print(2, "Remote: " + std::to_string(remote.getBattery()) + "%");
  /*
  const Position state{drive->getPosition()};
  remote.print(0, to_string(state.x));
  remote.print(1, to_string(state.y));
  remote.print(2, to_string(state.h));
  */
}

void Robot24In::driveControls() {
  const auto L = remote.getLStick();
  const auto R = remote.getRStick();
  drive->tank(L.second, R.second);
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_B)) {
    if(drive->getBrakeMode() == pros::E_MOTOR_BRAKE_COAST)
      drive->setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
    else
      drive->setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
  }
}
void Robot24In::intakeControls() {
  if(remote.getHold(pros::E_CONTROLLER_DIGITAL_Y)) {
    intake->preload();
    return;
  }
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_DOWN))
    intakeMacro = !intakeMacro;
  if(intakeMacro) {
    intakeMacroControls();
    if(remote.getRTrigger() == 1) return;
    switch(remote.getLTrigger()) {
      case 1: intake->manualExtend(); break;
      case -1: intake->manualRetract(); break;
      default: intake->manualStopArm(); break;
    }
  } else {
    intakeNoMacroControls();
    switch(remote.getLTrigger()) {
      case 1: intake->manualExtend(); break;
      case -1: intake->manualRetract(); break;
      default: intake->manualStopArm(); break;
    }
  }
}

void Robot24In::intakeMacroControls() {
  switch(remote.getRTrigger()) {
    case 1:
      if(intake->grabDetected())
        intake->stopRoller();
      else
        intake->intake();
      intake->extend();
      break;
    case -1:
      intake->outtake();
      intake->stopMacros();
      break;
    default:
      intake->stopRoller();
      intake->stopMacros();
      break;
  }
}

void Robot24In::intakeNoMacroControls() {
  switch(remote.getRTrigger()) {
    case 1: intake->intake(); break;
    case -1: intake->outtake(); break;
    default: intake->stopRoller(); break;
  }
}

void Robot24In::flywheelControls() {
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_UP)) flywheel->cycleSpeed();
}

void Robot24In::wingControls() {
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_LEFT)) leftWing->toggle();
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_RIGHT)) rightWing->toggle();
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_A)) {
    leftWing->toggle();
    rightWing->toggle();
  }
}

bool Robot24In::endGameControls() {
  if(!remote.getHold(pros::E_CONTROLLER_DIGITAL_X)) {
    endGame->move_voltage(0);
    return false;
  }
  intake->stopRoller();
  flywheel->setSpeed(0);
  switch(remote.getLTrigger()) {
    case 1: endGame->move_voltage(maxMotorVoltage); break;
    case -1: endGame->move_voltage(-maxMotorVoltage); break;
    default: endGame->move_voltage(0); break;
  }
  switch(remote.getRTrigger()) {
    case 1: endGamePiston->extend(); break;
    case -1: endGamePiston->retract(); break;
    default: break;
  }
  return true;
}
} // namespace atum