#include "robot24In.hpp"

namespace atum {
Robot24In::Robot24In(std::unique_ptr<Logger> iLogger) :
    Robot(std::move(iLogger)) {
  initializeDrive();
  initializeFlywheel();
  initializeIntake();
  initializeMisc();
  remote.print(0, "Initialized!");
  if(logger) logger->info("24\" robot initialized!");
}

void Robot24In::disabled() {
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

void Robot24In::initializeDrive() {
  // Initialize IMUs.
  std::unique_ptr<IMU> imu{
      std::make_unique<IMU>(std::initializer_list<std::uint8_t>{5, 6, 7},
                            false,
                            1.0,
                            Logger::makeLog(Logger::LoggerLevel::Info))};

  // Initialize odometry.
  const inch_t wheelCircumference{203.724231788_mm};
  std::unique_ptr<Odometer> r{
      std::make_unique<Odometer>('G', 'H', wheelCircumference, 3.25_in, true)};
  std::unique_ptr<Odometer> s{
      std::make_unique<Odometer>('E', 'F', wheelCircumference, -5.5_in)};
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
  drive = std::make_unique<Drive>(
      createMotorGroup({-11, 12, -14}),
      createMotorGroup({17, -19, 20}),
      Drive::Geometry{12.75_in, 12.566_in, 0.42857142857},
      std::move(odom),
      Logger::makeLog(Logger::LoggerLevel::Debug));
}

void Robot24In::initializeFlywheel() {
  const double highSpeed{406.69};
  const PID::Parameters pidParams{0.1, 0, 0, 22.25, std::make_pair(0, 12000)};
  flywheel =
      std::make_unique<Flywheel>(createMotorGroup({-1, 10}),
                                 std::initializer_list<double>{0.0, highSpeed},
                                 std::make_unique<PID>(pidParams),
                                 Logger::makeLog(Logger::LoggerLevel::Info));
}

void Robot24In::initializeIntake() {
  Intake::Parameters intakeParams{};
  intakeParams.armManualRetractVoltage = -8000;
  intakeParams.armManualExtendVoltage = maxMotorVoltage;
  intakeParams.loadingPosition = 0_deg;
  intakeParams.shootingPosition = 10_deg;
  intakeParams.extendedPosition = 105_deg;
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
          600_deg_per_s, 16500_deg_per_s_sq, 82500_deg_per_s_cb});
  auto extendProfile = std::make_unique<AngularSCurveProfile>(
      AngularSCurveProfile::MotionConstraints{
          600_deg_per_s, 16500_deg_per_s_sq, 82500_deg_per_s_cb});
  std::shared_ptr<Controller> retractPosition{
      std::make_shared<PID>(PID::Parameters{80, 0, 0, 0})};
  std::shared_ptr<Controller> retractVelocity{
      std::make_shared<PID>(PID::Parameters{40, 0, 0, 30})};
  std::shared_ptr<Controller> extendPosition{
      std::make_shared<PID>(PID::Parameters{80, 0, 0, 0})};
  std::shared_ptr<Controller> extendVelocity{
      std::make_shared<PID>(PID::Parameters{40, 0, 0, 30})};
  auto retractFollower = std::make_unique<AngularProfileFollower>(
      std::move(retractProfile), retractPosition, retractVelocity, 3, 0.5);
  auto extendFollower = std::make_unique<AngularProfileFollower>(
      std::move(extendProfile), extendPosition, extendVelocity, 3, 0.5);
  std::unique_ptr<Controller> balance{
      std::make_unique<PID>(PID::Parameters{150, 0, 0, 0})};
  std::unique_ptr<Controller> hold{
      std::make_unique<PID>(PID::Parameters{150, 0, 0, 0})};
  intake = std::make_unique<Intake>(
      createMotor(-3),
      createMotor(-15, pros::E_MOTOR_GEAR_RED),
      createMotor(18, pros::E_MOTOR_GEAR_RED),
      std::make_unique<pros::Distance>(16),
      nullptr,
      std::make_unique<pros::Distance>(2),
      std::make_unique<Potentiometer>(pros::ext_adi_port_pair_t{8, 'C'}),
      std::make_unique<Potentiometer>(pros::ext_adi_port_pair_t{8, 'B'}),
      std::move(retractFollower),
      std::move(extendFollower),
      std::move(balance),
      std::move(hold),
      intakeParams,
      Logger::makeLog(Logger::LoggerLevel::Info));
}

void Robot24In::initializeMisc() {
  endGame = createMotorGroup({-4, 9}, pros::E_MOTOR_GEAR_RED);
  leftWing = std::make_unique<Piston>(pros::ext_adi_port_pair_t{8, 'D'});
  rightWing = std::make_unique<Piston>(pros::ext_adi_port_pair_t{8, 'A'});
  glow = std::make_unique<pros::ADILED>(pros::ext_adi_port_pair_t{8, 'G'}, 64);
  glow->set_all(ledRed);
  wait(100_ms);
  glow->update();
  endGamePiston = std::make_unique<Piston>(pros::ext_adi_port_pair_t{8, 'F'});

  auto profileAngular = std::make_unique<AngularSCurveProfile>(
      AngularSCurveProfile::MotionConstraints{
          515_deg_per_s, 773_deg_per_s_sq, 3865_deg_per_s_cb});
  std::shared_ptr<Controller> positionAngular{
      std::make_shared<PID>(PID::Parameters{200, 0, 0, 0})};
  std::shared_ptr<Controller> velocityAngular{
      std::make_shared<PID>(PID::Parameters{100, 0, 0, 20})};
  auto angleFollower = std::make_unique<AngularProfileFollower>(
      std::move(profileAngular), positionAngular, velocityAngular, 10, 0.8);
  pointAt =
      std::make_shared<PointAt>(drive.get(), 0.9, std::move(angleFollower));

  const LateralSCurveProfile::MotionConstraints lateralConstraints{
      40_in_per_s, 80_in_per_s_sq, 400_in_per_s_cb};
  auto profileLateral =
      std::make_unique<LateralSCurveProfile>(lateralConstraints);
  std::shared_ptr<Controller> positionLateral{
      std::make_shared<PID>(PID::Parameters{10, 0, 0, 0})};
  std::shared_ptr<Controller> velocityLateral{
      std::make_shared<PID>(PID::Parameters{10, 0, 0, 200})};
  auto lateralFollower = std::make_unique<LateralProfileFollower>(
      std::move(profileLateral), positionLateral, velocityLateral, 50, 17);
  std::unique_ptr<Controller> straight{
      std::make_unique<PID>(PID::Parameters{100})};
  const tile_t distanceToSwitch{1_tile};
  moveTo = std::make_unique<MoveTo>(drive.get(),
                                    0.1,
                                    pointAt,
                                    std::move(lateralFollower),
                                    std::move(straight),
                                    distanceToSwitch);

  std::unique_ptr<Controller> left{
      std::make_unique<PID>(PID::Parameters{4, 0, 0, 40})};
  std::unique_ptr<Controller> right{
      std::make_unique<PID>(PID::Parameters{4, 0, 0, 40})};
  ramsete = std::make_unique<RAMSETE>(drive.get(),
                                      pointAt,
                                      std::move(left),
                                      std::move(right),
                                      lateralConstraints);
}
} // namespace atum