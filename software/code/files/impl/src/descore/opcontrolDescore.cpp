#include "robotDescore.hpp"

namespace atum {

void RobotDescore::opcontrol() {
  if(logger) logger->info("Operator control has started!");
  drive->setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
  while(true) {
    wait(10_ms);
    opcontrolRemotePrint();
    driveControls();
    intakeControls();
    wingControls();
  }
}

void RobotDescore::opcontrolRemotePrint() {
  remote.print(0, "Fortnite Balls");
  remote.print(
      1,
      "Brain: " +
          std::to_string(static_cast<int>(pros::c::battery_get_capacity())) +
          "%");
  remote.print(2, "Remote: " + std::to_string(remote.getBattery()) + "%");
}

void RobotDescore::driveControls() {
  const auto L = remote.getLStick();
  const auto R = remote.getRStick();
  drive->arcade(L.second, R.first);
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_Y)) {
    if(drive->getBrakeMode() == pros::E_MOTOR_BRAKE_COAST)
      drive->setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
    else
      drive->setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
  }
}

void RobotDescore::intakeControls() {
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_A)) {
    if(!intake->get_voltages()[0])
      intake->move_voltage(maxMotorVoltage);
    else
      intake->move_voltage(0);
  } else if(remote.getPress(pros::E_CONTROLLER_DIGITAL_B)) {
    if(!intake->get_voltages()[0])
      intake->move_voltage(-maxMotorVoltage);
    else
      intake->move_voltage(0);
  }
}

void RobotDescore::wingControls() {
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_L1)) leftWing->toggle();
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_R1)) rightWing->toggle();
  if(remote.getPress(pros::E_CONTROLLER_DIGITAL_X)) {
    leftWing->toggle();
    rightWing->toggle();
  }
}
} // namespace atum
