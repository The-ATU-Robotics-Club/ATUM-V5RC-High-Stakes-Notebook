#include "robot24In.hpp"

namespace atum {
void Robot24In::autonomous() {
  if(logger) logger->info("Autonomous control has started!");
  flywheel->setSpeed(0); // Flywheel should initially be off.
  remote.print(0, "Autonomous");
  drive->setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
  switch(GUI::getRoutine()) {
    case Routine::Quals: qualsAuton(); break;
    case Routine::Elims: elimsAuton(); break;
    case Routine::WP: wpAuton(); break;
    case Routine::Skills: skillsAuton(); break;
    default: nothingAuton(); break;
  }
}

void Robot24In::qualsAuton() {
  basicAuton();
  leftWing->retract();
  ramsete->go("QualsToGoal",
              false,
              {{2.45_tile, -1_tile, 180_deg},
               {1_tile, -2.5_tile, -90_deg},
               {-1_tile, -2.5_tile, -90_deg}},
              true,
              LateralSCurveProfile::MotionConstraints{35_in_per_s});
}

void Robot24In::elimsAuton() {
  basicAuton();
  moveTo->go({2.5_tile, 0_tile},
             false,
             LateralSCurveProfile::MotionConstraints{
                 40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
  moveTo->go({2.5_tile, -1.75_tile},
             true,
             LateralSCurveProfile::MotionConstraints{
                 40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
  moveTo->go({2.5_tile, 0_tile},
             false,
             LateralSCurveProfile::MotionConstraints{
                 40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
  moveTo->go({2.5_tile, -1.75_tile},
             true,
             LateralSCurveProfile::MotionConstraints{
                 40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
}

void Robot24In::wpAuton() {
  setupAutons();
  wait(2_s);
  moveTo->go({-2_tile, -2_tile}, true);
  moveTo->go({-1.5_tile, -2.45_tile});
  moveTo->go({0_tile, -2.5_tile});
}

void Robot24In::nothingAuton() {
  if(logger) logger->info("Nothing ran!");
}

void Robot24In::skillsAuton() {
  drive->setPosition({-2_tile - 4.5_in, 2_tile + 4.5_in, -45_deg});
  flywheel->setSpeed(1);
  intake->extend();
  for(std::size_t i{0}; i < 23; i++) {
    intake->preload();
    waitUntil(finishedPreloading(), 2_s);
  }
  intake->retract();
  flywheel->setSpeed(0);
  intake->stopRoller();
  moveTo->go({-2_tile, 2_tile}, true);
  moveTo->go({-1.5_tile, 2.45_tile});
  intake->extend();
  schedule({withinHere({0_tile, 2.5_tile}, 1_ft),
            [this]() {
              wait(1_s);
              intake->retract();
              rightWing->extend();
            },
            "Lift wings under goal.",
            10_s});
  leftWing->extend();
  ramsete->go("UnderGoal",
              false,
              {{-1.5_tile, 2.5_tile, 90_deg},
               {1_tile, 2.5_tile, 90_deg},
               {2.45_tile, 1_tile, 180_deg}},
              false,
              LateralSCurveProfile::MotionConstraints{35_in_per_s});
  leftWing->retract();
  rightWing->retract();
  moveTo->go({2.5_tile, 1.75_tile}, true);
  moveTo->go({2.5_tile, -1.5_tile});
  moveTo->go({2.5_tile, 1.75_tile}, true);
  schedule({withinHere({1_tile, 0.5_tile}, 3_ft),
            [this]() {
              intake->outtake();
              leftWing->extend();
              wait(2_s);
              intake->stopRoller();
              rightWing->extend();
            },
            "Lift wings for goal.",
            10_s});
  ramsete->go("SideToGoal",
              false,
              {{2.5_tile, 1.5_tile, -90_deg},
               {0.66_tile, 0.5_tile, 180_deg},
               {2.5_tile, -0.5_tile, 90_deg}},
              false,
              LateralSCurveProfile::MotionConstraints{35_in_per_s});
  leftWing->retract();
  rightWing->retract();
  moveTo->go({0.66_tile, 0_tile}, true);
  leftWing->extend();
  rightWing->extend();
  moveTo->go({2.5_tile, 0.25_tile});
  leftWing->retract();
  rightWing->retract();
  moveTo->go({0.66_tile, 0_tile}, true);
  leftWing->extend();
  rightWing->extend();
  moveTo->go({2.5_tile, -0.25_tile});
  leftWing->retract();
  rightWing->retract();
  moveTo->go({0.66_tile, 0_tile}, true);
  intake->outtake();
}

void Robot24In::basicAuton() {
  setupAutons();
  ramsete->go(
      "LineUp",
      false,
      {{-0.5_tile, -0.5_tile, -160_deg}, {-2.5_tile, -1.8_tile, -90_deg}},
      true);
  rightWing->extend();
  moveTo->go({-1.75_tile, -2.55_tile},
             false,
             LateralSCurveProfile::MotionConstraints{
                 40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
  moveTo->go({-2.55_tile, -1.95_tile},
             true,
             LateralSCurveProfile::MotionConstraints{
                 40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
  for(std::size_t i{0}; i < 8; i++) {
    moveTo->go({-1.75_tile, -2.5_tile},
               false,
               LateralSCurveProfile::MotionConstraints{
                   40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
    moveTo->go({-2.55_tile, -1.95_tile},
               true,
               LateralSCurveProfile::MotionConstraints{
                   40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
  }
  moveTo->go({-1.75_tile, -2.5_tile},
             false,
             LateralSCurveProfile::MotionConstraints{
                 40_in_per_s, 160_in_per_s_sq, 800_in_per_s_cb});
  moveTo->go({-1.5_tile, -2.5_tile});
  intake->extend();
  leftWing->extend();
  intake->outtake();
  schedule({withinHere({0_tile, -2.5_tile}, 1_ft),
            [this]() {
              leftWing->retract();
              wait(1.25_s);
              intake->retract();
              leftWing->extend();
              wait(1.5_s);
              intake->stopRoller();
            },
            "Lift wings under goal.",
            10_s});
  ramsete->go("QualsUnderGoal",
              false,
              {{-1_tile, -2.5_tile, 90_deg},
               {0.95_tile, -2.5_tile, 90_deg},
               {2.45_tile, -1_tile, 0_deg}},
              false,
              LateralSCurveProfile::MotionConstraints{35_in_per_s});
  rightWing->retract();
}

void Robot24In::setupAutons() {
  const Position initialPosition{-1_tile - 9_in, -2_tile - 13.75_in, 0_deg};
  drive->setPosition(initialPosition);
  schedule({[]() { return false; },
            [this]() {
              leftWing->extend();
              rightWing->extend();
            },
            "Open wings to push.",
            2.125_s});
  ramsete->go("GetOutOfWay",
              false,
              {initialPosition,
               {initialPosition.x, -2_tile, 0_deg},
               {-0.5_tile, -0.5_tile, 45_deg}});
  leftWing->retract();
  rightWing->retract();
  wait(0.75_s);
}

std::function<bool()> Robot24In::finishedGrabbing() {
  return [this]() { return !intake->isGrabbing(); };
}

std::function<bool()> Robot24In::finishedPreloading() {
  return [this]() { return !intake->isPreloading(); };
}

std::function<bool()> Robot24In::withinHere(const Position &position,
                                            const inch_t threshold) {
  return [this, position, threshold]() {
    return distance(drive->getPosition(), position) <= threshold;
  };
}
} // namespace atum
