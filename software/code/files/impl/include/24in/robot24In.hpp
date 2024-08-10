#pragma once

#include "atum/atum.hpp"
#include "robots/flywheel.hpp"
#include "robots/intake.hpp"

namespace atum {
class Robot24In : public Robot {
  public:
  Robot24In(std::unique_ptr<Logger> iLogger);

  void disabled() override;

  void opcontrol() override;

  void autonomous() override;

  private:
  void initializeDrive();
  void initializeFlywheel();
  void initializeIntake();
  void initializeMisc();

  void qualsAuton();
  void elimsAuton();
  void wpAuton();
  void nothingAuton();
  void skillsAuton();
  void basicAuton();
  void setupAutons();

  void opcontrolRemotePrint();
  void driveControls();
  void intakeControls();
  void intakeMacroControls();
  void intakeNoMacroControls();
  void flywheelControls();
  void wingControls();
  bool endGameControls();

  std::function<bool()> finishedGrabbing();
  std::function<bool()> finishedPreloading();
  std::function<bool()> withinHere(const Position &position,
                                   const inch_t threshold);

  Remote remote{pros::E_CONTROLLER_MASTER};
  std::unique_ptr<Drive> drive;
  std::unique_ptr<Flywheel> flywheel;
  std::unique_ptr<Intake> intake;
  std::unique_ptr<pros::MotorGroup> endGame;
  std::unique_ptr<Piston> leftWing;
  std::unique_ptr<Piston> rightWing;
  std::unique_ptr<Piston> endGamePiston;
  std::unique_ptr<pros::ADILED> glow;

  bool intakeMacro{true};

  std::shared_ptr<PointAt> pointAt;
  std::unique_ptr<MoveTo> moveTo;
  std::unique_ptr<RAMSETE> ramsete;
};
} // namespace atum
