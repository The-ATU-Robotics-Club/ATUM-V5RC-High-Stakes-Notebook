#pragma once

#include "atum/atum.hpp"

namespace atum {
class RobotDescore : public Robot {
  public:
  RobotDescore(std::unique_ptr<Logger> iLogger);

  void disabled() override;

  void opcontrol() override;

  void autonomous() override;

  private:
  void initializeDrive();
  void initializeIntake();
  void initializeMisc();

  void qualsAuton();
  void elimsAuton();
  void wpAuton();
  void nothingAuton();

  void opcontrolRemotePrint();
  void driveControls();
  void intakeControls();
  void wingControls();

  atum::Remote remote{pros::E_CONTROLLER_MASTER};
  std::unique_ptr<Drive> drive;
  std::unique_ptr<pros::MotorGroup> intake;
  std::unique_ptr<Piston> leftWing;
  std::unique_ptr<Piston> rightWing;
  std::unique_ptr<pros::MotorGroup> endGame;
};

} // namespace atum
