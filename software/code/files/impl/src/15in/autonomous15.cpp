#include "robot15In.hpp"

namespace atum {
void Robot15In::autonomous() {
  if(logger) logger->info("Autonomous control has started!");
  flywheel->setSpeed(0); // Flywheel should initially be off.
  drive->setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
  switch(GUI::getRoutine()) {
    case Routine::Quals: qualsAuton(); break;
    case Routine::Elims: elimsAuton(); break;
    case Routine::WP: wpAuton(); break;
    case Routine::Skills: skillsAuton(); break;
    default: nothingAuton(); break;
  }
}

void Robot15In::qualsAuton() {
  getWP();
  getTwoMoreTriballs();
  leftWing->extend();
  rightWing->extend();
  moveTo->go({2.5_tile, -0.4_tile}, true);
  moveTo->go({1_tile, -0.4_tile});
  intake->extend();
  flywheel->setSpecificSpeed(50);
  intake->intake();
  moveTo->go({0_tile, 1_in - 2_tile});
  intake->stopRoller();
  pointAt->go(-135_deg);
}

void Robot15In::elimsAuton() {
  getWP();
  getTwoMoreTriballs();
  intake->extend();
  intake->intake();
  intake->grab();
  moveTo->go({12_in, -0.0875_tile});
  waitUntil(finishedGrabbing(), 2_s);
  pointAt->go(95_deg);
  intake->removeBall();
  leftWing->extend();
  rightWing->extend();
  moveTo->go({2.5_tile, -0.4_tile}, true);
  moveTo->go({1_tile, -0.4_tile});
  pointAt->go(0_deg, true);
}

void Robot15In::wpAuton() {
  getWP();
  moveTo->go({2.5_tile, -1.55_tile}, true);
  intake->extend();
  flywheel->setSpecificSpeed(50);
  intake->intake();
  moveTo->go({0.375_tile, -1.5_tile});
  intake->stopRoller();
  intake->extend();
  pointAt->go(-135_deg);
}

void Robot15In::nothingAuton() {
  if(logger) logger->info("Nothing ran!");
}

void Robot15In::skillsAuton() {
  drive->setPosition({-2_tile - 6_in, -2_tile - 6_in, -135_deg});
  flywheel->setSpeed(1);
  intake->extend();
  for(std::size_t i{0}; i < 23; i++) {
    intake->preload();
    waitUntil(finishedPreloading(), 2_s);
  }
  intake->retract();
  flywheel->setSpeed(0);
  intake->stopRoller();
  moveTo->go({-2_tile, -2_tile}, true);
  moveTo->go({-1.5_tile, -2.45_tile}, true);
  schedule({withinHere({0_tile, -2.5_tile}, 1_ft),
            [this]() {
              wait(0.75_s);
              leftWing->extend();
              rightWing->extend();
              wait(1.25_s);
              leftWing->retract();
            },
            "Lift wings under goal.",
            10_s});
  ramsete->go("UnderGoal",
              false,
              {{-1.5_tile, -2.5_tile, 90_deg},
               {1.25_tile, -2.5_tile, 90_deg},
               {2.625_tile, -1_tile, 0_deg}},
              true,
              LateralSCurveProfile::MotionConstraints{35_in_per_s});
  for(std::size_t i{0}; i < 3; i++) {
    moveTo->go({2.5_tile, -1.8_tile});
    moveTo->go({2.625_tile, 1_tile}, true);
  }
  moveTo->go({2.5_tile, -1.8_tile});
}

void Robot15In::getWP() {
  drive->setPosition({-2_tile - 6_in, -2_tile - 6_in, -135_deg});
  intake->extend();
  wait(1_s);
  intake->grab();
  waitUntil(finishedGrabbing(), 1_s);
  moveTo->go({-2_tile, -2_tile}, true);
  moveTo->go({-1.5_tile, -2.45_tile}, true);
  schedule({withinHere({0_tile, -2.5_tile}, 1_ft),
            [this]() {
              wait(1_s);
              rightWing->extend();
              wait(1_s);
              rightWing->retract();
            },
            "Lift wings under goal.",
            10_s});
  ramsete->go("WPUnderGoal",
              false,
              {{-1.5_tile, -2.5_tile, 90_deg},
               {1.25_tile, -2.5_tile, 90_deg},
               {2.625_tile, -1_tile, 0_deg}},
              true,
              LateralSCurveProfile::MotionConstraints{35_in_per_s});
  moveTo->go({2.5_tile, -1.625_tile});
  leftWing->retract();
  rightWing->retract();
  moveTo->go({1.625_tile, -1.625_tile});
  pointAt->go(65_deg);
  intake->removeBall();
  intake->grab();
  moveTo->go({3_tile, -3_tile});
  waitUntil(finishedGrabbing(), 2_s);
  intake->stopMacros();
  moveTo->go({2_tile, -2_tile}, true);
  pointAt->go(30_deg);
  intake->removeBall();
  intake->retract();
  moveTo->go({2.5_tile, -1.5_tile});
  moveTo->go({2.65_tile, 1_tile});
}

void Robot15In::getTwoMoreTriballs() {
  moveTo->go({2.5_tile, -1.666_tile}, true);
  intake->extend();
  pointAt->go({3_in, -1_tile});
  intake->grab();
  moveTo->go({0.75_tile, -0.975_tile});
  waitUntil(finishedGrabbing(), 1_s);
  pointAt->go(75_deg);
  intake->removeBall();
  intake->intake();
  intake->grab();
  moveTo->go({1.1_tile, -0.125_tile});
  waitUntil(finishedGrabbing(), 1_s);
  pointAt->go(100_deg);
  intake->removeBall();
}

std::function<bool()> Robot15In::finishedGrabbing() {
  return [this]() { return !intake->isGrabbing(); };
}

std::function<bool()> Robot15In::finishedPreloading() {
  return [this]() { return !intake->isPreloading(); };
}

std::function<bool()> Robot15In::withinHere(const Position &position,
                                            const inch_t threshold) {
  return [this, position, threshold]() {
    return distance(drive->getPosition(), position) <= threshold;
  };
}
} // namespace atum