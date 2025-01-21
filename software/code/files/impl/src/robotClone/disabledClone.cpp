#include "robotClone.hpp"
#include <memory>

namespace atum {
RobotClone::RobotClone(const int iID) : Robot{this}, id{iID} {
  if(id == ID15) {
    driveSetup15();
    ladybrownSetup15();
    intakeSetup15();
    goalClampSetup15();
    autonSetup15();
  } else if(id == ID24) {
    driveSetup24();
    ladybrownSetup24();
    intakeSetup24();
    goalClampSetup24();
    autonSetup24();
  }
}

void RobotClone::disabled() {
  intake->startBackgroundTasks();
  ladybrown->startBackgroundTasks();
}

void RobotClone::driveSetup15() {
  std::unique_ptr<Motor> leftDriveMtr{std::make_unique<Motor>(
      MotorPortsList{-7, -8, 9, -10},
      Motor::Gearing{pros::v5::MotorGears::blue, 48.0 / 36.0},
      "left drive")};
  std::unique_ptr<Motor> rightDriveMtr{std::make_unique<Motor>(
      MotorPortsList{1, -2, 3, 4},
      Motor::Gearing{pros::v5::MotorGears::blue, 48.0 / 36.0},
      "right drive")};
  const inch_t wheelCircumference{203.724231788_mm};
  gps = std::make_unique<GPS>(
      17, Pose{0.224_in, 6.46_in, -90_deg}, 0.5, 1.0, Logger::Level::Debug);
  std::unique_ptr<Odometer> forwardOdometer{
      std::make_unique<Odometer>('A', 'B', wheelCircumference, -0.209_in)};
  std::unique_ptr<Odometer> sideOdometer{
      std::make_unique<Odometer>('C', 'D', wheelCircumference, 1.791_in, true)};
  std::unique_ptr<IMU> imu{std::make_unique<IMU>(PortsList{13, 16})};
  std::unique_ptr<Odometry> odometry{
      std::make_unique<Odometry>(std::move(forwardOdometer),
                                 std::move(sideOdometer),
                                 std::move(imu),
                                 Logger::Level::Debug)};
  odometry->startBackgroundTasks();
  drive = std::make_unique<Drive>(std::move(leftDriveMtr),
                                  std::move(rightDriveMtr),
                                  std::move(odometry),
                                  Drive::Geometry{11.862_in, 10.21_in});
}

void RobotClone::ladybrownSetup15() {
  std::unique_ptr<Motor> leftLadybrownMotor{
      std::make_unique<Motor>(MotorPortsList{-18},
                              Motor::Gearing{pros::v5::MotorGears::green, 5},
                              "left ladybrown")};
  std::unique_ptr<Motor> rightLadybrownMotor{
      std::make_unique<Motor>(MotorPortsList{11},
                              Motor::Gearing{pros::v5::MotorGears::green, 5},
                              "right ladybrown")};
  std::unique_ptr<Piston> ladybrownPiston{std::make_unique<Piston>('E')};
  std::unique_ptr<RotationSensor> ladybrownRotation{
      std::make_unique<RotationSensor>(15, true)};
  std::unique_ptr<LineTracker> ladybrownLineTracker{
      std::make_unique<LineTracker>('F', 2700)};
  std::unordered_map<LadybrownState, std::optional<degree_t>>
      ladybrownPositions{{LadybrownState::Resting, -11.1_deg},
                         {LadybrownState::Loading, 7_deg},
                         {LadybrownState::Preparing, 60_deg},
                         {LadybrownState::Scoring, 135_deg}};
  Ladybrown::Parameters ladybrownParameters{
      6, -5_deg, 50_deg, ladybrownPositions, 0_s};
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
                                          std::move(ladybrownLineTracker),
                                          ladybrownParameters,
                                          std::move(profileFollower));
}

void RobotClone::intakeSetup15() {
  std::unique_ptr<Motor> intakeMtr{std::make_unique<Motor>(
      MotorPortsList{5, -6}, Motor::Gearing{pros::v5::MotorGears::blue})};
  std::vector<ColorSensor::HueField> hueFields{
      {ColorSensor::Color::Red, 10, 30}, {ColorSensor::Color::Blue, 216, 30}};
  std::unique_ptr<ColorSensor> colorSensor{
      std::make_unique<ColorSensor>(19, hueFields)};
  Intake::Parameters intakeParams;
  intakeParams.jamVelocity = 20_rpm;
  intakeParams.timerUntilJamChecks = Timer{0.25_s};
  intakeParams.timeUntilUnjammed = 0.25_s;
  intakeParams.sortThrowTime = 0.05_s;
  intakeParams.pressLoadTime = 0.1_s;
  intakeParams.finishLoadingTime = 0.1_s;
  intakeParams.generalTimeout = 1_s;
  intake = std::make_unique<Intake>(std::move(intakeMtr),
                                    std::move(colorSensor),
                                    ladybrown.get(),
                                    intakeParams);
}

void RobotClone::goalClampSetup15() {
  std::unique_ptr<Piston> goalClampPiston{std::make_unique<Piston>('G', true, true)};
  goalClamp = std::make_unique<GoalClamp>(std::move(goalClampPiston), nullptr, nullptr);
  goalRush = std::make_unique<Piston>('Z');
  goalRushClamp = std::make_unique<Piston>('Z');
}

void RobotClone::autonSetup15() {
  // Path follower setup.
  Path::setDefaultParams(
      {1, 76.5_in_per_s, 76.5_in_per_s_sq, drive->getGeometry().track});
  AcceptableDistance acceptable{forever};
  PID::Parameters pathFollowerPIDParams{0.031, 0, 0, 0.031};
  pathFollowerPIDParams.ffScaling = true;
  std::unique_ptr<Controller> left{
      std::make_unique<PID>(pathFollowerPIDParams)};
  std::unique_ptr<Controller> right{
      std::make_unique<PID>(pathFollowerPIDParams)};
  pathFollower =
      std::make_unique<PathFollower>(drive.get(),
                                     acceptable,
                                     std::move(left),
                                     std::move(right),
                                     AccelerationConstants{0.5, 1.6},
                                     PathFollower::FeedbackParameters{},
                                     Logger::Level::Debug);

  // Turn setup.
  AngularProfile::Parameters turnMotionParams{
      720_deg_per_s, 10000_deg_per_s_sq, 10000_deg_per_s_cb};
  turnMotionParams.usePosition = true;
  AngularProfile turnProfile{turnMotionParams};
  // Timeout here gets set by the follower, so don't worry about the "forever."
  AcceptableAngle turnAcceptable{forever, 1_deg};
  PID::Parameters turnPIDParams{1.0, 0, 0, 0.85};
  turnPIDParams.ffScaling = true;
  std::unique_ptr<Controller> turnVelocityController =
      std::make_unique<PID>(turnPIDParams);
  const AccelerationConstants kA{0.7, 0.1};
  std::unique_ptr<Controller> turnPositionController =
      std::make_unique<PID>(PID::Parameters{48.0, 0.0, 0.0, 0.0, 0.0});
  std::unique_ptr<AngularProfileFollower> profileFollower{
      std::make_unique<AngularProfileFollower>(
          turnProfile,
          turnAcceptable,
          std::move(turnVelocityController),
          kA,
          std::move(turnPositionController),
          5_deg)};
  turn = std::make_unique<Turn>(drive.get(), std::move(profileFollower));
}

void RobotClone::driveSetup24() {
  std::unique_ptr<Motor> leftDriveMtr{std::make_unique<Motor>(
      MotorPortsList{-7, -8, -9, 10},
      Motor::Gearing{pros::v5::MotorGears::blue, 48.0 / 36.0},
      "left drive")};
  std::unique_ptr<Motor> rightDriveMtr{std::make_unique<Motor>(
      MotorPortsList{1, 2, 3, -4},
      Motor::Gearing{pros::v5::MotorGears::blue, 48.0 / 36.0},
      "right drive")};
  const inch_t wheelCircumference{203.724231788_mm};
  std::unique_ptr<Odometer> forwardOdometer{
      std::make_unique<Odometer>('C', 'D', wheelCircumference, -0.209_in)};
  std::unique_ptr<Odometer> sideOdometer{
      std::make_unique<Odometer>('E', 'F', wheelCircumference, 1.791_in)};
  std::unique_ptr<IMU> imu{std::make_unique<IMU>(PortsList{11, 18})};
  std::unique_ptr<Odometry> odometry{
      std::make_unique<Odometry>(std::move(forwardOdometer),
                                 std::move(sideOdometer),
                                 std::move(imu),
                                 Logger::Level::Debug)};
  odometry->startBackgroundTasks();
  drive = std::make_unique<Drive>(std::move(leftDriveMtr),
                                  std::move(rightDriveMtr),
                                  std::move(odometry),
                                  Drive::Geometry{11.862_in, 10.21_in});
}

void RobotClone::ladybrownSetup24() {
  std::unique_ptr<Motor> leftLadybrownMotor{
      std::make_unique<Motor>(MotorPortsList{-15},
                              Motor::Gearing{pros::v5::MotorGears::green, 5},
                              "left ladybrown")};
  std::unique_ptr<Motor> rightLadybrownMotor{
      std::make_unique<Motor>(MotorPortsList{16},
                              Motor::Gearing{pros::v5::MotorGears::green, 5},
                              "right ladybrown")};
  std::unique_ptr<Piston> ladybrownPiston{std::make_unique<Piston>('B')};
  std::unique_ptr<RotationSensor> ladybrownRotation{
      std::make_unique<RotationSensor>()};
  std::unique_ptr<LineTracker> ladybrownLineTracker{
      std::make_unique<LineTracker>('H', 2700)};
  std::unordered_map<LadybrownState, std::optional<degree_t>>
      ladybrownPositions{{LadybrownState::Resting, -11.1_deg},
                         {LadybrownState::Loading, 7_deg},
                         {LadybrownState::Preparing, 60_deg},
                         {LadybrownState::Scoring, 125_deg}};
  Ladybrown::Parameters ladybrownParameters{
      6, -10_deg, 50_deg, ladybrownPositions, 0.375_s};
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
                                          std::move(ladybrownLineTracker),
                                          ladybrownParameters,
                                          std::move(profileFollower));
}

void RobotClone::intakeSetup24() {
  std::unique_ptr<Motor> intakeMtr{std::make_unique<Motor>(
      MotorPortsList{-5, 6}, Motor::Gearing{pros::v5::MotorGears::blue})};
  std::vector<ColorSensor::HueField> hueFields{
      {ColorSensor::Color::Red, 10, 30}, {ColorSensor::Color::Blue, 216, 30}};
  std::unique_ptr<ColorSensor> colorSensor{
      std::make_unique<ColorSensor>(17, hueFields)};
  Intake::Parameters intakeParams;
  intakeParams.jamVelocity = 20_rpm;
  intakeParams.timerUntilJamChecks = Timer{0.25_s};
  intakeParams.timeUntilUnjammed = 0.25_s;
  intakeParams.sortThrowTime = 0.05_s;
  intakeParams.pressLoadTime = 0.1_s;
  intakeParams.finishLoadingTime = 0.1_s;
  intakeParams.generalTimeout = 1_s;
  intake = std::make_unique<Intake>(std::move(intakeMtr),
                                    std::move(colorSensor),
                                    ladybrown.get(),
                                    intakeParams);
}

void RobotClone::goalClampSetup24() {
  std::unique_ptr<Piston> goalClampPiston{std::make_unique<Piston>('H', true, true)};
  goalClamp = std::make_unique<GoalClamp>(std::move(goalClampPiston), nullptr, nullptr);
  goalRush = std::make_unique<Piston>('Z');
  goalRushClamp = std::make_unique<Piston>('Z');
}

void RobotClone::autonSetup24() {
  // Path follower setup.
  Path::setDefaultParams(
      {1, 40_in_per_s, 40_in_per_s_sq, drive->getGeometry().track});
  AcceptableDistance acceptable{forever, 1_in};
  PID::Parameters pathFollowerPIDParams{0.031, 0, 0, 0.031};
  pathFollowerPIDParams.ffScaling = true;
  std::unique_ptr<Controller> left{
      std::make_unique<PID>(pathFollowerPIDParams)};
  std::unique_ptr<Controller> right{
      std::make_unique<PID>(pathFollowerPIDParams)};
  pathFollower =
      std::make_unique<PathFollower>(drive.get(),
                                     acceptable,
                                     std::move(left),
                                     std::move(right),
                                     AccelerationConstants{0.5, 1.6},
                                     PathFollower::FeedbackParameters{},
                                     Logger::Level::Debug);

  // Turn setup.
  AngularProfile::Parameters turnMotionParams{
      720_deg_per_s, 10000_deg_per_s_sq, 10000_deg_per_s_cb};
  turnMotionParams.usePosition = true;
  AngularProfile turnProfile{turnMotionParams};
  // Timeout here gets set by the follower, so don't worry about the "forever."
  AcceptableAngle turnAcceptable{forever, 1_deg};
  PID::Parameters turnPIDParams{1.0, 0, 0, 0.85};
  turnPIDParams.ffScaling = true;
  std::unique_ptr<Controller> turnVelocityController =
      std::make_unique<PID>(turnPIDParams);
  const AccelerationConstants kA{0.7, 0.1};
  std::unique_ptr<Controller> turnPositionController =
      std::make_unique<PID>(PID::Parameters{48.0, 0.0, 0.0, 0.0, 0.0});
  std::unique_ptr<AngularProfileFollower> profileFollower{
      std::make_unique<AngularProfileFollower>(
          turnProfile,
          turnAcceptable,
          std::move(turnVelocityController),
          kA,
          std::move(turnPositionController),
          5_deg)};
  turn = std::make_unique<Turn>(drive.get(), std::move(profileFollower));
}
} // namespace atum