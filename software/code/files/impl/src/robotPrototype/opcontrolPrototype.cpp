#include "robotPrototype.hpp"

namespace atum {
void RobotPrototype::opcontrol() {
  while(true) {
    const double forward{remote.getLStick().y};
    const double turn{remote.getRStick().x};
    leftMotors.moveVoltage(forward + turn);
    rightMotors.moveVoltage(forward - turn);

    if(remote.getHold(Remote::Button::Up)) {
      climbMotors.moveVoltage(12);
    } else if(remote.getHold(Remote::Button::Down)) {
      climbMotors.moveVoltage(-12);
    } else {
      climbMotors.brake();
    }

    wait(10_ms);
  }
}
}