#include "robot15In.hpp"

namespace atum {
Robot15In::Robot15In(std::unique_ptr<Logger> iLogger) :
    Robot(std::move(iLogger)) {
  initializeDrive();
  initializeFlywheel();
  initializeIntake();
  initializeMisc();
  remote.print(0, "Initialized!");
  if(logger) logger->info("15\" robot initialized!");
}

void Robot15In::disabled() {
  while(true) {
    switch(GUI::getColor()) {
      case Color::Red: glow->set_all(ledRed); break;
      case Color::Blue: glow->set_all(ledBlue); break;
    }
    glow->update();
    drive->setPosition({0.0_tile, 0.0_tile, 0.0_deg});
    wait(100_ms);
  }
}

void Robot15In::initializeDrive() {
  // Initialize IMUs.
  std::unique_ptr<IMU> imu{
      std::make_unique<IMU>(std::initializer_list<std::uint8_t>{14, 20},
                            false,
                            1.0,
                            Logger::makeLog(Logger::LoggerLevel::Info))};

  // Initialize odometry.
  const inch_t wheelCircumference{203.724231788_mm};
  std::unique_ptr<Odometer> r{std::make_unique<Odometer>(
      'A', 'B', wheelCircumference, -3.375_in, true)};
  std::unique_ptr<Odometer> s{
      std::make_unique<Odometer>('C', 'D', wheelCircumference, 5.175_in, true)};
  std::unique_ptr<SideEncoderOdometry> odom{
      std::make_unique<SideEncoderOdometry>(
          nullptr,
          std::move(r),
          std::move(s),
          std::move(imu),
          Logger::makeLog(Logger::LoggerLevel::Info))};
  odom->setPosition({0_tile, 0_tile, 0_deg});
  odom->start();

  // Initialize drive.
  drive = std::make_unique<Drive>(createMotorGroup({-1, 2, -3}),
                                  createMotorGroup({4, -5, 6}),
                                  Drive::Geometry{11.75_in, 10.21_in, 0.6},
                                  std::move(odom),
                                  Logger::makeLog(Logger::LoggerLevel::Info));
}

void Robot15In::initializeFlywheel() {
  const double highSpeed{450};
  const PID::Parameters pidParams{0.1, 0, 0, 21.12, std::make_pair(0, 12000)};
  flywheel =
      std::make_unique<Flywheel>(createMotorGroup({11, -12}),
                                 std::initializer_list<double>{0.0, highSpeed},
                                 std::make_unique<PID>(pidParams),
                                 Logger::makeLog(Logger::LoggerLevel::Info));
}

void Robot15In::initializeIntake() {
  Intake::Parameters intakeParams{};
  intakeParams.armManualRetractVoltage = -8000;
  intakeParams.armManualExtendVoltage = maxMotorVoltage;
  intakeParams.loadingPosition = 47.5_deg;
  intakeParams.shootingPosition = 59.5_deg;
  intakeParams.extendedPosition = 110_deg;
  intakeParams.intakeInVoltage = maxMotorVoltage;
  intakeParams.intakeOutVoltage = -maxMotorVoltage;
  intakeParams.preloadDistance = 250_mm;
  intakeParams.preloadTimeout = 1_s;
  intakeParams.preloadDelay = 0.25_s;
  intakeParams.intakeDelay = 0.0_s;
  intakeParams.shootTime = 0.1_s;
  intakeParams.grabDistance = 150_mm;
  auto retractProfile = std::make_unique<AngularSCurveProfile>(
      AngularSCurveProfile::MotionConstraints{
          600_deg_per_s, 11000_deg_per_s_sq, 55000_deg_per_s_cb});
  auto extendProfile = std::make_unique<AngularSCurveProfile>(
      AngularSCurveProfile::MotionConstraints{
          600_deg_per_s, 11000_deg_per_s_sq, 55000_deg_per_s_cb});
  std::shared_ptr<Controller> retractPosition{
      std::make_shared<PID>(PID::Parameters{80, 0, 0, 0})};
  std::shared_ptr<Controller> retractVelocity{
      std::make_shared<PID>(PID::Parameters{40, 0, 0, 30})};
  std::shared_ptr<Controller> extendPosition{
      std::make_shared<PID>(PID::Parameters{80, 0, 0, 0})};
  std::shared_ptr<Controller> extendVelocity{
      std::make_shared<PID>(PID::Parameters{40, 0, 0, 30})};
  auto retractFollower = std::make_unique<AngularProfileFollower>(
      std::move(retractProfile), retractPosition, retractVelocity, 1.5, 1.5);
  auto extendFollower = std::make_unique<AngularProfileFollower>(
      std::move(extendProfile), extendPosition, extendVelocity, 1.5, 1.5);
  std::unique_ptr<Controller> balance{
      std::make_unique<PID>(PID::Parameters{25, 0, 0, 0})};
  std::unique_ptr<Controller> hold{
      std::make_unique<PID>(PID::Parameters{75, 0, 0, 0})};
  intake = std::make_unique<Intake>(
      createMotor(9),
      createMotor(-7, pros::E_MOTOR_GEAR_RED),
      createMotor(8, pros::E_MOTOR_GEAR_RED),
      std::make_unique<pros::Distance>(16),
      std::make_unique<pros::Distance>(19),
      std::make_unique<pros::Distance>(21),
      std::make_unique<Potentiometer>(pros::ext_adi_port_pair_t{18, 'H'}),
      std::make_unique<Potentiometer>(pros::ext_adi_port_pair_t{18, 'E'}),
      std::move(retractFollower),
      std::move(extendFollower),
      std::move(balance),
      std::move(hold),
      intakeParams,
      Logger::makeLog(Logger::LoggerLevel::Info));
}

void Robot15In::initializeMisc() {
  leftWing = std::make_unique<Piston>(pros::ext_adi_port_pair_t{18, 'F'});
  rightWing = std::make_unique<Piston>(pros::ext_adi_port_pair_t{18, 'G'});
  glow = std::make_unique<pros::ADILED>('G', 64);
  glow->set_all(ledRed);
  wait(100_ms);
  glow->update();

  auto profileAngular = std::make_unique<AngularSCurveProfile>(
      AngularSCurveProfile::MotionConstraints{
          515_deg_per_s, 773_deg_per_s_sq, 3865_deg_per_s_cb});
  std::shared_ptr<Controller> positionAngular{
      std::make_shared<PID>(PID::Parameters{200, 0, 0, 0})};
  std::shared_ptr<Controller> velocityAngular{
      std::make_shared<PID>(PID::Parameters{100, 0, 0, 20})};
  auto angleFollower = std::make_unique<AngularProfileFollower>(
      std::move(profileAngular), positionAngular, velocityAngular, 6.375, 1);
  pointAt =
      std::make_shared<PointAt>(drive.get(), 0.9, std::move(angleFollower));

  const LateralSCurveProfile::MotionConstraints lateralConstraints{
      41_in_per_s, 82_in_per_s_sq, 410_in_per_s_cb};
  auto profileLateral =
      std::make_unique<LateralSCurveProfile>(lateralConstraints);
  std::shared_ptr<Controller> positionLateral{
      std::make_shared<PID>(PID::Parameters{20, 0, 0, 0})};
  std::shared_ptr<Controller> velocityLateral{
      std::make_shared<PID>(PID::Parameters{20, 0, 0, 185})};
  auto lateralFollower = std::make_unique<LateralProfileFollower>(
      std::move(profileLateral), positionLateral, velocityLateral, 40, 20);
  std::unique_ptr<Controller> straight{
      std::make_unique<PID>(PID::Parameters{50})};
  const tile_t distanceToSwitch{1_tile};
  moveTo = std::make_unique<MoveTo>(drive.get(),
                                    0.1,
                                    pointAt,
                                    std::move(lateralFollower),
                                    std::move(straight),
                                    distanceToSwitch);

  std::unique_ptr<Controller> left{
      std::make_unique<PID>(PID::Parameters{3.1, 0, 0, 30.5})};
  std::unique_ptr<Controller> right{
      std::make_unique<PID>(PID::Parameters{3.1, 0, 0, 30.5})};
  ramsete = std::make_unique<RAMSETE>(drive.get(),
                                      pointAt,
                                      std::move(left),
                                      std::move(right),
                                      lateralConstraints);
}
} // namespace atum