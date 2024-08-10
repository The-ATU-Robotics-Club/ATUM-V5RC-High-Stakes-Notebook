#include "robotDescore.hpp"

namespace atum {
void RobotDescore::autonomous() {
  if(logger) logger->info("Autonomous control has started!");
  remote.print(0, "Autonomous");
  drive->setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
  switch(GUI::getRoutine()) {
    case Routine::Quals: qualsAuton(); break;
    case Routine::Elims: elimsAuton(); break;
    case Routine::WP: wpAuton(); break;
    default: nothingAuton(); break;
  }
}

void RobotDescore::qualsAuton() {}

void RobotDescore::elimsAuton() {
  qualsAuton();
}

void RobotDescore::wpAuton() {}

void RobotDescore::nothingAuton() {
  if(logger) logger->info("Nothing ran!");
}
} // namespace atum
