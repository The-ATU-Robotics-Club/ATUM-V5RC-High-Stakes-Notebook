#include "robotClone.hpp"
#include "robots/sharedSystems/ladybrown.hpp"

namespace atum {
void RobotClone::opcontrol() {
  matchTimer.setTime();
  matchTimer.setAlarm(1_min + 15_s);
  // Where the first routine should be skills.
  if(GUI::Routines::selectedRoutine() == 0) {
    matchTimer.setAlarm(45_s);
  }
  Schedule fifteenAwayScheduled{{"Rumble at 15s Away",
                                 matchTimer.checkGoneOff(),
                                 [=]() { remote.rumble("---"); }}};
  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    intake->setSortOutColor(ColorSensor::Color::Blue);
  } else {
    intake->setSortOutColor(ColorSensor::Color::Red);
  }
  drive->setBrakeMode(pros::MotorBrake::coast);
  while(true) {
    gps->getPose();

    const double forward{speedMultiplier * remote.getLStick().y};
    const double turn{remote.getRStick().x};
    drive->arcade(forward, turn);

    remotePrinting();

    speedMultiplier = 1.0;
    if(useHangControls) {
      hangControls();
    } else if(useManualControls) {
      manualControls();
    } else if(useLadybrownControls) {
      ladybrownControls();
    } else {
      intakeControls();
    }

    configurationControls();

    if(remote.getPress(Remote::Button::Y)) {
      goalClamp->toggleClamp();
    }

    if(remote.getPress(Remote::Button::Left)) {
      goalRush->toggle();
    }

    if(remote.getPress(Remote::Button::A)) {
      goalRushClamp->toggle();
    }

    if(remote.getHold(Remote::Button::Up) &&
       remote.getHold(Remote::Button::Down)) {
      GUI::Manager::easteregg();
    }

    wait();
  }
}

void RobotClone::remotePrinting() {
  remote.print(
      0,
      "BRAIN: " +
          std::to_string(static_cast<int>(pros::battery::get_capacity())) +
          "%");
  remote.print(1, "SORT: " + toString(intake->getSortOutColor()));
}

void RobotClone::manualControls() {
  remote.print(2, "MODE: Manual");

  switch(remote.getLTrigger()) {
    case -1: ladybrown->retract(); break;
    case 1: ladybrown->extend(); break;
    default: ladybrown->stop(); break;
  }

  switch(remote.getRTrigger()) {
    case -1: intake->outtake(); break;
    case 1: intake->intake(); break;
    default: intake->stop(); break;
  }
}

void RobotClone::ladybrownControls() {
  remote.print(2, "MODE: Ladybrown");

  switch(remote.getLTrigger()) {
    case -1:
      if(ladybrown->noRingDetection() || !ladybrown->hasRing()) {
        ladybrown->rest();
      }
      break;
    case 1:
      if(ladybrown->readyToScore()) {
        scored = true;
        ladybrown->score();
      }
      break;
    default:
      if(scored) {
        scored = false;
        ladybrown->finishScore();
      }
      break;
  }

  switch(remote.getRTrigger()) {
    case -1: intake->outtake(); break;
    case 1: intake->load(); break;
    default: intake->stop(); break;
  }

  /*switch(remote.getTrigger()) {
    case -1: rusharm->retract(); break;
    case 1: rusharm->extend(); break;
    default: rusharm->stop(); break;
  }*/

}

void RobotClone::intakeControls() {
  remote.print(2, "MODE: Intake");

  // Only go down if you know it's okay to go down.
  if(!ladybrown->noRingDetection() && !ladybrown->hasRing()) {
    ladybrown->rest();
  }

  switch(remote.getRTrigger()) {
    case -1: intake->outtake(); break;
    case 1:
      if(remote.getHold(Remote::Button::L1) || goalClamp->hasGoal()) {
        intake->intake();
      } else {
        intake->index();
      }
      break;
    default: intake->stop(); break;
  }
}

void RobotClone::hangControls() {
  remote.print(2, "MODE: Hang");

  if(remote.getHold(Remote::Button::L1)) {
    speedMultiplier = 0.45;
    ladybrown->prepare();
  } else {
    ladybrown->fullyExtend();
  }
  switch(remote.getRTrigger()) {
    case -1: intake->outtake(); break;
    case 1: intake->intake(); break;
    default: intake->stop(); break;
  }
}

void RobotClone::configurationControls() {
  if(remote.getPress(Remote::Button::X)) {
    useManualControls = !useManualControls;
    useHangControls = false; // Hang controls have the lowest precedence.
  }

  if(remote.getPress(Remote::Button::Right)) {
    useLadybrownControls = !useLadybrownControls;
    useHangControls = false; // Hang controls have the lowest precedence.
  }

  if(remote.getPress(Remote::Button::Up)) {
    useHangControls = !useHangControls;
  }

  if(remote.getPress(Remote::Button::Down)) {
    const ColorSensor::Color currentColor{intake->getSortOutColor()};
    const ColorSensor::Color nextColor{(static_cast<int>(currentColor) + 1) %
                                       3};
    intake->setSortOutColor(nextColor);
  }

  if(remote.getPress(Remote::Button::B)) {
    if(drive->getBrakeMode() == pros::MotorBrake::coast) {
      drive->setBrakeMode(pros::MotorBrake::hold);
    } else {
      drive->setBrakeMode(pros::MotorBrake::coast);
    }
  }
}
} // namespace atum