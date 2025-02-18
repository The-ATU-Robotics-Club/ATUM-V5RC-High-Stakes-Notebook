#include "robotClone.hpp"

namespace atum {
void RobotClone::opcontrol() {
  setSortToOpposite();
  matchTimer.setTime();
  matchTimer.setAlarm(1_min + 10_s);
  // Where the first routine should be skills.
  if(GUI::Routines::selectedRoutine() == 0) {
    matchTimer.setAlarm(40_s);
    intake->setSortOutColor(ColorSensor::Color::None);
    goalClamp->unclamp();
    if(id == ID15) {
      intake->outtake();
      wait(0.1_s);
      intake->stop();
    }
  }
  scheduler.schedule({"Rumble at 15s Away", matchTimer.checkGoneOff(), [=]() {
                        remote.rumble("---");
                      }});
  drive->setBrakeMode(pros::MotorBrake::coast);
  while(true) {
    const double forward{speedMultiplier * remote.getLStick().y};
    const double turn{remote.getRStick().x};
    drive->arcade(forward, turn);

    visualFeedback();

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
      goalRush->toggleArm();
    }

    if(remote.getPress(Remote::Button::A)) {
      goalRush->toggleClamp();
    }

    if(remote.getHold(Remote::Button::Up) &&
       remote.getHold(Remote::Button::Down)) {
      GUI::Manager::easteregg();
    }

    wait(20_ms);
  }
}

void RobotClone::visualFeedback() {
  remote.print(
      0, std::string{"CLAMP: "} + (goalClamp->isClamped() ? "Down" : "Up"));
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
      if(!ladybrown->hasRing()) {
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

  if(goalClamp->hasGoal() && !recentlyUnclamped) {
    goalClamp->clamp();
    recentlyUnclamped = true;
  } else if(!goalClamp->hasGoal()) {
    recentlyUnclamped = false;
  }
}

void RobotClone::intakeControls() {
  remote.print(2, "MODE: Intake");

  // Only go down if you know it's okay to go down.
  if(!ladybrown->hasRing()) {
    ladybrown->rest();
  }

  switch(remote.getRTrigger()) {
    case -1: intake->outtake(); break;
    case 1:
      if(remote.getHold(Remote::Button::L1) || goalClamp->isClamped()) {
        intake->intake();
      } else {
        intake->index();
      }
      break;
    default: intake->stop(); break;
  }

  if(remote.getHold(Remote::Button::L2)) {
    goalClamp->unclamp();
  } else if(goalClamp->hasGoal() && !recentlyUnclamped) {
    goalClamp->clamp();
    recentlyUnclamped = true;
  } else if(!goalClamp->hasGoal()) {
    recentlyUnclamped = false;
  }
}

void RobotClone::hangControls() {
  remote.print(2, "MODE: Hang");

  if(remote.getHold(Remote::Button::L1)) {
    speedMultiplier = 0.45;
    ladybrown->prepare();
    goalRush->retractArm();
  } else {
    ladybrown->fullyExtend();
    goalRush->extendArm();
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