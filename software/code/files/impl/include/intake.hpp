#pragma once

#include "atum/atum.hpp"

namespace atum {
class Intake : public Task {
  public:
  struct Parameters {
    degree_t loadingPosition;
    degree_t shootingPosition;
    degree_t extendedPosition;
    double intakeInVoltage;
    double intakeOutVoltage;
    double armManualRetractVoltage;
    double armManualExtendVoltage;
    inch_t preloadDistance;
    second_t preloadTimeout;
    second_t preloadDelay;
    second_t intakeDelay;
    second_t shootTime;
    inch_t grabDistance;
  };

  enum class HoldState { NA, AllowExtend, AllowRetract };
  enum class MacroState { NA, Preloading, Grabbing };

  Intake(std::unique_ptr<pros::Motor> iRoller,
         std::unique_ptr<pros::Motor> iArmL,
         std::unique_ptr<pros::Motor> iArmR,
         std::unique_ptr<pros::Distance> iPreload1,
         std::unique_ptr<pros::Distance> iPreload2,
         std::unique_ptr<pros::Distance> iGrab,
         std::unique_ptr<Potentiometer> iPotL,
         std::unique_ptr<Potentiometer> iPotR,
         std::unique_ptr<AngularProfileFollower> iRetractFollower,
         std::unique_ptr<AngularProfileFollower> iExtendFollower,
         std::unique_ptr<Controller> iBalance,
         std::unique_ptr<Controller> iHold,
         const Parameters &iParams,
         std::unique_ptr<Logger> iLogger = nullptr);

  void removeBall(const second_t removeTime = 0.75_s);

  void runRoller(const double voltage);
  void stopRoller();

  void intake();
  void outtake();

  void runArm(double voltage);
  void stopArm();

  void manualExtend();
  void manualRetract();
  void manualStopArm();
  void extend();
  void retract();
  void armTo(const degree_t iDesired);

  void stopMacros();

  void preload();
  bool isPreloading() const;

  void grab();
  bool isGrabbing() const;

  void setHold(const HoldState &iHoldState);

  bool preloadDetected() const;
  bool grabDetected() const;

  private:
  void taskFn1() override;
  void taskFn2() override;

  void blockingRunArm(degree_t target, const MacroState whileState);

  void preloadMacro();
  void grabMacro();

  degree_t getArmPosition() const;
  degrees_per_second_t getArmVelocity();

  void setBrakeMode(const pros::motor_brake_mode_e_t brakemode);

  std::unique_ptr<pros::Motor> roller;
  std::unique_ptr<pros::Motor> armL;
  std::unique_ptr<pros::Motor> armR;
  std::unique_ptr<pros::Distance> preload1Eye;
  std::unique_ptr<pros::Distance> preload2Eye;
  std::unique_ptr<pros::Distance> grabEye;
  std::unique_ptr<Potentiometer> potL;
  std::unique_ptr<Potentiometer> potR;
  std::unique_ptr<AngularProfileFollower> retractFollower;
  std::unique_ptr<AngularProfileFollower> extendFollower;
  std::unique_ptr<Controller> balance;
  std::unique_ptr<Controller> hold;
  const Parameters params;
  std::unique_ptr<Logger> logger;

  MacroState macroState{MacroState::NA};
  HoldState holdState{HoldState::AllowRetract};

  SlewRate test{1000};

  double armVoltage{0.0};
  degree_t holdPosition{0_deg};

  std::optional<degree_t> desired;
};
} // namespace atum