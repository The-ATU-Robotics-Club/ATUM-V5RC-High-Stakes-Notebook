#include "robotPrototype.hpp"

namespace atum {
RobotPrototype::RobotPrototype() : Robot{this} {
    climbMotors.setBrakeMode(pros::v5::MotorBrake::brake);
}

void RobotPrototype::disabled() {}
} // namespace atum