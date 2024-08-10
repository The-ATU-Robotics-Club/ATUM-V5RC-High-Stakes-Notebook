#include "robotDescore.hpp"

namespace atum {
RobotDescore::RobotDescore(std::unique_ptr<Logger> iLogger) :
    Robot(std::move(iLogger)) {
  initializeDrive();
  initializeIntake();
  initializeMisc();
  remote.print(0, "Initialized!");
  if(logger) logger->info("RobotDescore robot initialized!");
}

void RobotDescore::disabled() {
  while(true) {
    wait(100_ms);
  }
}

void RobotDescore::initializeDrive() {
  drive = std::make_unique<Drive>(createMotorGroup({-11, 12, -13, -14}),
                                  createMotorGroup({17, 18, -19, 20}),
                                  Drive::Geometry{11.25_in, 10.21_in, 0.75},
                                  nullptr,
                                  Logger::makeLog(Logger::LoggerLevel::Info));
}

void RobotDescore::initializeIntake() {
  intake = createMotorGroup({1, -10});
}

void RobotDescore::initializeMisc() {
  leftWing = std::make_unique<Piston>('B');
  rightWing = std::make_unique<Piston>('C');
  endGame = createMotorGroup({-15, 16}, pros::E_MOTOR_GEAR_RED);
}
} // namespace atum