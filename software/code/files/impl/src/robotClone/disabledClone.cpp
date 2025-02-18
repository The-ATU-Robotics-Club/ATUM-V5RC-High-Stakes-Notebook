#include "atum/depend/units.h"
#include "atum/motion/motionProfile.hpp"
#include "robotClone.hpp"

namespace atum {
RobotClone::RobotClone(const int iID) : Robot{this}, id{iID} {
  if(id == ID15) {
    driveSetup15();
    ladybrownSetup15();
    intakeSetup15();
    goalSetup15();
  } else if(id == ID24) {
    driveSetup24();
    ladybrownSetup24();
    intakeSetup24();
    goalSetup24();
  }
  autonSetup();
  intake->startBackgroundTasks();
  ladybrown->startBackgroundTasks();
}

void RobotClone::disabled() {
  wait(100_ms);
}

void RobotClone::driveSetup15() {
  std::unique_ptr<Motor> leftDriveMtr{std::make_unique<Motor>(
      MotorPortsList{-7, 8, -9, -10},
      Motor::Gearing{pros::v5::MotorGears::blue, 48.0 / 36.0},
      "left drive")};
  std::unique_ptr<Motor> rightDriveMtr{std::make_unique<Motor>(
      MotorPortsList{1, -2, 3, 4},
      Motor::Gearing{pros::v5::MotorGears::blue, 48.0 / 36.0},
      "right drive")};
  drive = std::make_unique<Drive>(std::move(leftDriveMtr),
                                  std::move(rightDriveMtr),
                                  Drive::Geometry{11.862_in, 10.213335_in});

  const inch_t wheelCircumference{198_mm};
  std::unique_ptr<Odometer> forwardOdometer{
      std::make_unique<Odometer>('G', 'H', wheelCircumference, 0.086_in, true)};
  std::unique_ptr<Odometer> sideOdometer{std::make_unique<Odometer>(
      'C', 'D', wheelCircumference, -1.685_in, true)};
  std::unique_ptr<IMU> imu{std::make_unique<IMU>(PortsList{14, 17})};
  std::unique_ptr<Odometry> odometry{
      std::make_unique<Odometry>(std::move(forwardOdometer),
                                 std::move(sideOdometer),
                                 std::move(imu),
                                 drive.get())};
  odometry->startBackgroundTasks();
  drive->setTracker(std::move(odometry));
}

void RobotClone::ladybrownSetup15() {
  std::unique_ptr<Motor> leftLadybrownMotor{
      std::make_unique<Motor>(MotorPortsList{-19},
                              Motor::Gearing{pros::v5::MotorGears::green, 5},
                              "left ladybrown")};
  std::unique_ptr<Motor> rightLadybrownMotor{
      std::make_unique<Motor>(MotorPortsList{11},
                              Motor::Gearing{pros::v5::MotorGears::green, 5},
                              "right ladybrown")};
  std::unique_ptr<Piston> ladybrownPiston{std::make_unique<Piston>('B')};
  std::unique_ptr<RotationSensor> ladybrownRotation{
      std::make_unique<RotationSensor>(16, true)};
  std::unique_ptr<LimitSwitch> ladybrownSwitch{
      std::make_unique<LimitSwitch>(ADIExtenderPort{21, 'G'}, 2)};
  std::unordered_map<LadybrownState, std::optional<degree_t>>
      ladybrownPositions{{LadybrownState::Resting, -11.1_deg},
                         {LadybrownState::Loading, 9_deg},
                         {LadybrownState::Preparing, 60_deg},
                         {LadybrownState::Scoring, 135_deg}};
  Ladybrown::Parameters ladybrownParameters{
      6, -5_deg, 50_deg, ladybrownPositions, 0.125_s};
  ladybrownParameters.kG = 0.2;
  ladybrownParameters.holdController = PID{{0.3}};
  ladybrownParameters.balanceController = PID{{0.2}};
  ladybrownParameters.manualSlew = SlewRate{0.3};
  AngularProfile::Parameters ladybrownMotionParams{
      240_deg_per_s, 10000_deg_per_s_sq, 5000_deg_per_s_cb};
  ladybrownMotionParams.usePosition = true;
  AngularProfile ladybrownProfile{ladybrownMotionParams};
  // Timeout here gets set by the follower, so don't worry about the "forever."
  AcceptableAngle ladybrownAcceptable{forever, 3_deg};
  PID::Parameters ladybrownPIDParams{0.15, 0, 0, 1.65};
  ladybrownPIDParams.ffScaling = true;
  std::unique_ptr<Controller> ladybrownVelocityController =
      std::make_unique<PID>(ladybrownPIDParams);
  const AccelerationConstants kA{0.44, 0.1};
  std::unique_ptr<Controller> ladybrownPositionController =
      std::make_unique<PID>(PID::Parameters{0.15});
  std::unique_ptr<AngularProfileFollower> profileFollower =
      std::make_unique<AngularProfileFollower>(
          ladybrownProfile,
          ladybrownAcceptable,
          std::move(ladybrownVelocityController),
          kA,
          std::move(ladybrownPositionController));
  ladybrown = std::make_unique<Ladybrown>(std::move(leftLadybrownMotor),
                                          std::move(rightLadybrownMotor),
                                          std::move(ladybrownPiston),
                                          std::move(ladybrownRotation),
                                          std::move(ladybrownSwitch),
                                          ladybrownParameters,
                                          std::move(profileFollower));
}

void RobotClone::intakeSetup15() {
  std::unique_ptr<Motor> intakeMtr{std::make_unique<Motor>(
      MotorPortsList{5, -6}, Motor::Gearing{pros::v5::MotorGears::blue})};
  std::vector<ColorSensor::HueField> hueFields{
      {ColorSensor::Color::Red, 10, 30}, {ColorSensor::Color::Blue, 215, 30}};
  std::unique_ptr<ColorSensor> colorSensor{
      std::make_unique<ColorSensor>(PortsList{20}, hueFields)};
  Intake::Parameters intakeParams;
  intakeParams.jamVelocity = 20_rpm;
  intakeParams.timerUntilJamChecks = Timer{0.25_s};
  intakeParams.timeUntilUnjammed = 0.3_s;
  intakeParams.sortThrowTime = 0.05_s;
  intakeParams.pressLoadTime = 10_ms;
  intakeParams.finishLoadingTime = 0.1_s;
  intakeParams.generalTimeout = 1_s;
  intakeParams.intakingVoltage = 12.0;
  intake = std::make_unique<Intake>(std::move(intakeMtr),
                                    std::move(colorSensor),
                                    ladybrown.get(),
                                    intakeParams);
}

void RobotClone::goalSetup15() {
  // Setup goal clamp.
  std::unique_ptr<Piston> goalClampPiston{
      std::make_unique<Piston>('E', true, true)};
  std::unique_ptr<LimitSwitch> limitSwitch1{
      std::make_unique<LimitSwitch>(ADIExtenderPort{21, 'F'})};
  std::unique_ptr<LimitSwitch> limitSwitch2{
      std::make_unique<LimitSwitch>(ADIExtenderPort{21, 'E'})};
  goalClamp = std::make_unique<GoalClamp>(std::move(goalClampPiston),
                                          std::move(limitSwitch1),
                                          std::move(limitSwitch2));
  //  Setup goal rush.
  std::unique_ptr<Piston> goalRushArm{std::make_unique<Piston>('A')};
  std::unique_ptr<Piston> goalRushClamp{std::make_unique<Piston>('F')};
  std::unique_ptr<LimitSwitch> limitSwitchRush{
      std::make_unique<LimitSwitch>(ADIExtenderPort{21, 'C'})};
  goalRush = std::make_unique<GoalRush>(std::move(goalRushArm),
                                        std::move(goalRushClamp),
                                        std::move(limitSwitchRush));
}

void RobotClone::driveSetup24() {
  std::unique_ptr<Motor> leftDriveMtr{std::make_unique<Motor>(
      MotorPortsList{-6, 7, -8, -9},
      Motor::Gearing{pros::v5::MotorGears::blue, 48.0 / 36.0},
      "left drive")};
  std::unique_ptr<Motor> rightDriveMtr{std::make_unique<Motor>(
      MotorPortsList{-1, 2, 3, 5},
      Motor::Gearing{pros::v5::MotorGears::blue, 48.0 / 36.0},
      "right drive")};
  drive = std::make_unique<Drive>(std::move(leftDriveMtr),
                                  std::move(rightDriveMtr),
                                  Drive::Geometry{11.862_in, 10.213335_in});

  const inch_t wheelCircumference{198_mm};
  std::unique_ptr<Odometer> forwardOdometer{
      std::make_unique<Odometer>('C', 'D', wheelCircumference, 0.086_in, true)};
  std::unique_ptr<Odometer> sideOdometer{
      std::make_unique<Odometer>('G', 'H', wheelCircumference, -1.685_in)};
  std::unique_ptr<IMU> imu{std::make_unique<IMU>(PortsList{13, 14})};
  std::unique_ptr<Odometry> odometry{
      std::make_unique<Odometry>(std::move(forwardOdometer),
                                 std::move(sideOdometer),
                                 std::move(imu),
                                 drive.get())};
  odometry->startBackgroundTasks();
  drive->setTracker(std::move(odometry));
}

void RobotClone::ladybrownSetup24() {
  std::unique_ptr<Motor> leftLadybrownMotor{
      std::make_unique<Motor>(MotorPortsList{-20},
                              Motor::Gearing{pros::v5::MotorGears::green, 5},
                              "left ladybrown")};
  std::unique_ptr<Motor> rightLadybrownMotor{
      std::make_unique<Motor>(MotorPortsList{12},
                              Motor::Gearing{pros::v5::MotorGears::green, 5},
                              "right ladybrown")};
  std::unique_ptr<Piston> ladybrownPiston{std::make_unique<Piston>('A')};
  std::unique_ptr<RotationSensor> ladybrownRotation{
      std::make_unique<RotationSensor>(15, false)};
  std::unique_ptr<LimitSwitch> ladybrownSwitch{
      std::make_unique<LimitSwitch>(ADIExtenderPort{21, 'H'}, 2)};
  std::unordered_map<LadybrownState, std::optional<degree_t>>
      ladybrownPositions{{LadybrownState::Resting, -11.1_deg},
                         {LadybrownState::Loading, 20_deg},
                         {LadybrownState::Preparing, 60_deg},
                         {LadybrownState::Scoring, 125_deg}};
  Ladybrown::Parameters ladybrownParameters{
      6, -5_deg, 50_deg, ladybrownPositions, 0.125_s};
  ladybrownParameters.kG = 0.2;
  ladybrownParameters.holdController = PID{{0.3}};
  ladybrownParameters.balanceController = PID{{0.2}};
  ladybrownParameters.manualSlew = SlewRate{0.3};
  AngularProfile::Parameters ladybrownMotionParams{
      240_deg_per_s, 10000_deg_per_s_sq, 5000_deg_per_s_cb};
  ladybrownMotionParams.usePosition = true;
  AngularProfile ladybrownProfile{ladybrownMotionParams};
  // Timeout here gets set by the follower, so don't worry about the "forever."
  AcceptableAngle ladybrownAcceptable{forever, 3_deg};
  PID::Parameters ladybrownPIDParams{0.15, 0, 0, 1.65};
  ladybrownPIDParams.ffScaling = true;
  std::unique_ptr<Controller> ladybrownVelocityController =
      std::make_unique<PID>(ladybrownPIDParams);
  const AccelerationConstants kA{0.44, 0.1};
  std::unique_ptr<Controller> ladybrownPositionController =
      std::make_unique<PID>(PID::Parameters{0.15});
  std::unique_ptr<AngularProfileFollower> profileFollower =
      std::make_unique<AngularProfileFollower>(
          ladybrownProfile,
          ladybrownAcceptable,
          std::move(ladybrownVelocityController),
          kA,
          std::move(ladybrownPositionController));
  ladybrown = std::make_unique<Ladybrown>(std::move(leftLadybrownMotor),
                                          std::move(rightLadybrownMotor),
                                          std::move(ladybrownPiston),
                                          std::move(ladybrownRotation),
                                          std::move(ladybrownSwitch),
                                          ladybrownParameters,
                                          std::move(profileFollower));
}

void RobotClone::intakeSetup24() {
  std::unique_ptr<Motor> intakeMtr{std::make_unique<Motor>(
      MotorPortsList{-10, 11}, Motor::Gearing{pros::v5::MotorGears::blue})};
  std::vector<ColorSensor::HueField> hueFields{
      {ColorSensor::Color::Red, 10, 30}, {ColorSensor::Color::Blue, 215, 30}};
  std::unique_ptr<ColorSensor> colorSensor{
      std::make_unique<ColorSensor>(PortsList{17, 19}, hueFields)};
  Intake::Parameters intakeParams;
  intakeParams.jamVelocity = 20_rpm;
  intakeParams.timerUntilJamChecks = Timer{0.25_s};
  intakeParams.timeUntilUnjammed = 0.3_s;
  intakeParams.sortThrowTime = 0.05_s;
  intakeParams.pressLoadTime = 50_ms;
  intakeParams.finishLoadingTime = 0.1_s;
  intakeParams.generalTimeout = 1_s;
  intakeParams.intakingVoltage = 12.0;
  intake = std::make_unique<Intake>(std::move(intakeMtr),
                                    std::move(colorSensor),
                                    ladybrown.get(),
                                    intakeParams);
}

void RobotClone::goalSetup24() {
  // Setup goal clamp.
  std::unique_ptr<Piston> goalClampPiston{
      std::make_unique<Piston>('F', false, false)};
  std::unique_ptr<LimitSwitch> limitSwitch1{
      std::make_unique<LimitSwitch>(ADIExtenderPort{21, 'A'})};
  std::unique_ptr<LimitSwitch> limitSwitch2{
      std::make_unique<LimitSwitch>(ADIExtenderPort{21, 'B'})};
  goalClamp = std::make_unique<GoalClamp>(std::move(goalClampPiston),
                                          std::move(limitSwitch1),
                                          std::move(limitSwitch2));
  // Setup goal rush.
  std::unique_ptr<Piston> goalRushArm{std::make_unique<Piston>('B')};
  std::unique_ptr<Piston> goalRushClamp{std::make_unique<Piston>('E')};
  std::unique_ptr<LimitSwitch> limitSwitchRush{
      std::make_unique<LimitSwitch>(ADIExtenderPort{21, 'D'})};
  goalRush = std::make_unique<GoalRush>(std::move(goalRushArm),
                                        std::move(goalRushClamp),
                                        std::move(limitSwitchRush));
}

void RobotClone::autonSetup() {
  meters_per_second_t maxV{76.5_in_per_s};
  meters_per_second_squared_t maxA{153_in_per_s_sq};

  // Turn setup.
  AngularProfile::Parameters turnMotionParams{
      720_deg_per_s, 10000_deg_per_s_sq, 10000_deg_per_s_cb};
  turnMotionParams.usePosition = true;
  AngularProfile turnProfile{turnMotionParams};
  // Timeout here gets set by the follower, so don't worry about the "forever."
  AcceptableAngle turnAcceptable{forever, 3_deg, 1.5_rpm};
  PID::Parameters turnPIDParams{2.5, 0, 0, 0.875};
  turnPIDParams.ffScaling = true;
  std::unique_ptr<Controller> turnVelocityController =
      std::make_unique<PID>(turnPIDParams);
  const AccelerationConstants turnKA{0.75, 0.1};
  std::unique_ptr<Controller> turnPositionController =
      std::make_unique<PID>(PID::Parameters{40.0});
  std::unique_ptr<AngularProfileFollower> angularProfileFollower{
      std::make_unique<AngularProfileFollower>(
          turnProfile,
          turnAcceptable,
          std::move(turnVelocityController),
          turnKA,
          std::move(turnPositionController))};
  turn = std::make_unique<Turn>(drive.get(), std::move(angularProfileFollower));

  // Move to setup.
  LateralProfile::Parameters moveToMotionParams{maxV, maxA, 612_in_per_s_cb};
  moveToMotionParams.usePosition = true;
  LateralProfile moveToProfile{moveToMotionParams};
  AcceptableDistance moveToAcceptable{forever, 1.5_in, 1.5_in_per_s};
  std::unique_ptr<PID> directionController =
      std::make_unique<PID>(PID::Parameters{0.35});
  PID::Parameters moveToVelocityPIDParams{6, 0, 0, 6};
  moveToVelocityPIDParams.ffScaling = true;
  std::unique_ptr<Controller> moveToVelocityPID{
      std::make_unique<PID>(moveToVelocityPIDParams)};
  const AccelerationConstants kA{2.5, 1.25};
  std::unique_ptr<PID> moveToPositionPID =
      std::make_unique<PID>(PID::Parameters{60});
  std::unique_ptr<LateralProfileFollower> lateralProfileFollower{
      std::make_unique<LateralProfileFollower>(moveToProfile,
                                               moveToAcceptable,
                                               std::move(moveToVelocityPID),
                                               kA,
                                               std::move(moveToPositionPID),
                                               3_in)};
  moveTo = std::make_unique<MoveTo>(drive.get(),
                                    turn.get(),
                                    std::move(lateralProfileFollower),
                                    std::move(directionController));

  // Path follower setup.
  Path::setDefaultParams(
      {1_tile, maxV, maxA, maxA, drive->getGeometry().track});
  AcceptableDistance acceptable{forever};
  std::unique_ptr<Controller> forwardController{
      std::make_unique<PID>(moveToVelocityPIDParams)};
  std::unique_ptr<Controller> turnController =
      std::make_unique<PID>(PID::Parameters{14});
  pathFollower = std::make_unique<PathFollower>(drive.get(),
                                                acceptable,
                                                std::move(forwardController),
                                                std::move(turnController),
                                                kA,
                                                1_ft,
                                                Logger::Level::Debug);
}
} // namespace atum