#include "atum/devices/colorSensor.hpp"
#include "atum/time/time.hpp"
#include "robotClone.hpp"

namespace atum {
// Max drive velocity: 76.5 in. / s.
// Max drive acceleration: 153 in. / s^2.

// General Constants
static const LateralProfile::Parameters goalClampProfile{35_in_per_s,
                                                         70_in_per_s_sq};
static const LateralProfile::Parameters cornerMotionProfile{30_in_per_s,
                                                            90_in_per_s_sq};
LateralProfile::Parameters doubleStackProfile{40_in_per_s, 60_in_per_s_sq};
static const inches_per_second_t singleRingSpeed{60_in_per_s};
static const tile_t pushDoubleStackY{2.47_tile};
static const second_t goalRushDelay{100_ms};
static const second_t goalClampDelay{100_ms};
static const second_t singleRingDelay{0.825_s};
static const second_t doubleRingDelay{1.125_s};

// DATA:
// ODOMS DRIVE
// 12.2878 9.39187
// 12.334 9.40656
// 12.3254 9.4

/*
  _ ___ _ _   ___ _   _ _ _
 / | __( | ) / __| |_(_) | |___
 | |__ \V V  \__ \ / / | | (_-<
 |_|___/     |___/_\_\_|_|_/__/

*/
void RobotClone::skills15() {

  
  setupRoutine(
    {-2.5_tile + 7.5_in, 1_tile, 90_deg});
  intake->setSortOutColor(ColorSensor::Color::Blue);
  intake->index();
  moveTo->forward({-1_tile, 1_tile});
  wait(singleRingDelay);
  clampWhenReady();
  moveTo->reverse({-1_tile, 2_tile}, goalClampProfile);
  goalClamp->clamp();
  wait(goalClampDelay);
  intake->intake();

    moveTo->forward({-0.25_tile, 2_tile});
    wait(singleRingDelay);
    moveTo->reverse({-1_tile, 2_tile});

  moveTo->forward({-2_tile, 2_tile});
  wait(.25_s);
  moveTo->forward({-2.75_tile, 2.75_tile},
                  LateralProfile::Parameters{30_in_per_s, 30_in_per_s_sq});
  wait(singleRingDelay);
  moveTo->reverse({-2_tile, 2_tile});
  moveTo->reverse({-2.8_tile, 2.8_tile},
                  LateralProfile::Parameters{40_in_per_s, 40_in_per_s_sq});
  wait(1_s);
  goalClamp->unclamp();
  moveTo->forward({-2.25_tile, 2.25_tile});
  turn->toward(80_deg);

  intake->load();

  pathFollower->follow({{AcceptableDistance{3_s},
                         {0.5_tile, 2.55_tile, 90_deg},
                         false,
                         Path::Parameters{{0.5_tile, 3_tile}, 50_in_per_s}}});
  wait(.25_s);
  moveTo->reverse({0_tile, 2_tile},
                  LateralProfile::Parameters{50_in_per_s, 50_in_per_s_sq});
  wait(.25_s);
  moveTo->forward({0_tile, 2.625_tile},
                  LateralProfile::Parameters{60_in_per_s, 60_in_per_s_sq});
  wait(singleRingDelay);
  intake->stop();
  drive->arcade(0.5, 0);
  ladybrown->fullyExtend();
  waitUntil(ladybrown->checkStateIs(LadybrownState::Idle), 3_s);
  wait(2_s);
  moveTo->reverse({0_tile, 1.5_tile});
  ladybrown->rest();
  wait(0.25_s);
  clampWhenReady();
  moveTo->reverse({1_tile, 1_tile},
                  LateralProfile::Parameters{50_in_per_s, 50_in_per_s_sq});
  goalClamp->clamp();
  wait(1_s);
  intake->intake();
  moveTo->forward({1_tile, 2_tile},
                  LateralProfile::Parameters{50_in_per_s, 50_in_per_s_sq});
  wait(1_s);
  turn->toward({2_tile, 1_tile});
  moveTo->forward({2_tile, 1_tile},
                  LateralProfile::Parameters{40_in_per_s, 40_in_per_s_sq});
  wait(1_s);
  moveTo->forward({2_tile, 2_tile});
  wait(1_s);
  for(int i{0}; i < 2; i++) {
    moveTo->forward({3_tile, 3_tile},
                    LateralProfile::Parameters{40_in_per_s, 40_in_per_s_sq});
    wait(.5_s);
    moveTo->reverse({2_tile, 2_tile});
    wait(.5_s);
  }
  moveTo->reverse({2.8_tile, 2.8_tile},
                  LateralProfile::Parameters{40_in_per_s, 40_in_per_s_sq});
  goalClamp->unclamp();
  wait(.5_s);
  intake->index();
  moveTo->forward({2.55_tile, -0.75_tile},
                  LateralProfile::Parameters{50_in_per_s, 50_in_per_s_sq});
  wait(1_s);
  clampWhenReady();
  moveTo->reverse({1.8_tile, .2_tile}, goalClampProfile);
  wait(1_s);
  goalClamp->clamp();
  wait(goalClampDelay);
  intake->intake();
  moveTo->reverse({1.5_tile, 1.5_tile});
  goalClamp->unclamp();
  intake->stop();
  ladybrown->fullyExtend();
  goalRush->extendArm();
  moveTo->reverse({1_tile, 1_tile});
  moveTo->reverse({.25_tile, .25_tile},
                  LateralProfile::Parameters{50_in_per_s, 50_in_per_s_sq});
  moveTo->forward({0.5_tile, 0.5_tile});
}

/*
      ___ _ _  _ _   ___ _   _ _ _
     |_  ) | |( | ) / __| |_(_) | |___
      / /|_  _|V V  \__ \ / / | | (_-<
     /___| |_|      |___/_\_\_|_|_/__/

*/
void RobotClone::skills24() {
  setupRoutine({-2.5_tile, 0_tile, 90_deg});
  intake->setSortOutColor(ColorSensor::Color::Blue);
  intake->index();
  moveTo->forward({-2_tile, 0_tile});
  wait(.25_s);
  moveTo->reverse({-2.45_tile, 0_tile});
  intake->intake();
  wait(singleRingDelay);
  intake->stop();
  intake->index();
  moveTo->forward({-2_tile, -2_tile});
  wait(singleRingDelay);
  clampWhenReady();
  moveTo->reverse({-0.75_tile, -2_tile}, goalClampProfile);
  goalClamp->clamp();
  wait(goalClampDelay);
  intake->intake();
  wait(singleRingDelay);
  moveTo->forward({-1_tile, -.75_tile});
  wait(singleRingDelay);
  moveTo->forward({0_tile, -1.75_tile}); // x -2 -> 0
  wait(singleRingDelay);
  moveTo->forward({-2_tile, -2_tile});
  wait(singleRingDelay);
  moveTo->forward({-2.75_tile, -2.75_tile});
  wait(singleRingDelay);
  moveTo->reverse({-2_tile, -2_tile});
  moveTo->reverse({-2.75_tile, -2.75_tile});
  goalClamp->unclamp();
  moveTo->forward({-2.375_tile, -2.375_tile});
  turn->toward(100_deg);
  intake->stop();
  wait(singleRingDelay);
  intake->load();
  pathFollower->follow({{AcceptableDistance{3_s},
                         {0.5_tile, -2.55_tile, 90_deg},
                         false,
                         Path::Parameters{{0.5_tile, 3_tile}, 50_in_per_s}}});
  wait(.25_s);
  moveTo->reverse({0_tile, -2_tile});
  wait(.25_s);
  moveTo->forward({0_tile, -2.6_tile});
  wait(singleRingDelay);
  intake->stop();
  drive->arcade(0.5, 0);
  ladybrown->fullyExtend();
  waitUntil(ladybrown->checkStateIs(LadybrownState::Idle), 3_s);
  wait(1_s);
  moveTo->reverse({0_tile, -1.75_tile});
  ladybrown->rest();
  intake->index();
  moveTo->forward({1_tile, -2_tile});
  clampWhenReady();
  moveTo->reverse({1_tile, -1_tile},
                  LateralProfile::Parameters{30_in_per_s, 30_in_per_s_sq});
  goalClamp->clamp();
  wait(goalClampDelay);
  intake->intake();
  moveTo->forward({2_tile, -1_tile});
  wait(500_ms);
  intake->intake();
  moveTo->forward({2_tile, -2_tile});
  wait(500_ms);
  intake->intake();
  for(int i{0}; i < 3; i++) {
    moveTo->forward({2.75_tile, -2.75_tile});
    moveTo->reverse({2.25_tile, -2.25_tile});
    wait(500_ms);
  }
  moveTo->reverse({2.75_tile, -2.75_tile});
  wait(100_ms);
  goalClamp->unclamp();
  moveTo->forward({2_tile, -2_tile});
  ladybrown->fullyExtend();
  goalRush->extendArm();
  intake->stop();
  moveTo->reverse({.25_tile, -.25_tile},
                  LateralProfile::Parameters{50_in_per_s, 50_in_per_s_sq});
  moveTo->forward({.5_tile, -.5_tile});
  intake->stop();
  wait(2_s);
}

ROUTINE_DEFINITIONS_FOR(RobotClone) {
  START_ROUTINE("Skills")
  if(id == ID15) {
    skills15();
  } else if(id == ID24) {
    skills24();
  }
  END_ROUTINE

  START_ROUTINE("Negative Side")
  setupRoutine(
      {-2.5_tile + 7.5_in + (id == ID15 ? 0_in : 7.5_in), 1_tile, 90_deg});
  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    rushLeft(2_tile);
  } else {
    rushRight(2_tile);
  }
  endOfNegativeRoutines();
  END_ROUTINE

  START_ROUTINE("Negative Mid")
  setupRoutine(
      {-2.5_tile + 7.5_in + (id == ID15 ? 0_in : 7.5_in), 1_tile, 90_deg});
  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    rushRight(2_tile);
  } else {
    rushLeft(2_tile);
  }
  endOfNegativeRoutines();
  END_ROUTINE

  START_ROUTINE("Positive Side")
  setupRoutine(
      {-2.5_tile + 7.5_in + (id == ID15 ? 0_in : 7.5_in), -1_tile, 90_deg});
  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    rushRight();
  } else {
    rushLeft();
  }
  endOfPositiveRoutines();
  END_ROUTINE

  START_ROUTINE("Positive Mid")
  setupRoutine(
      {-2.5_tile + 7.5_in + (id == ID15 ? 0_in : 7.5_in), -1_tile, 90_deg});
  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    rushLeft();
  } else {
    rushRight();
  }
  endOfPositiveRoutines();
  END_ROUTINE

  START_ROUTINE("Do Nothing")
  setupRoutine({});
  END_ROUTINE
}

/*
  _      ___         _
 | |    | _ \_  _ __| |_
 | |__  |   / || (_-< ' \
 |____| |_|_\\_,_/__/_||_|

*/
void RobotClone::rushLeft(const tile_t extraY) {
  intake->setSortOutColor(ColorSensor::Color::None);
  goalRush->extendArm();
  goalRush->release();
  intake->index();
  goalRushWhenReady();
  const tile_t rushYAdj{
      extraY -
      (GUI::Routines::selectedColor() == MatchColor::Blue ? 1_tile : 0_tile)};
  degree_t rushH{30_deg};
  if(GUI::Routines::selectedColor() == MatchColor::Blue) {
    rushH = 180_deg - rushH;
  }
  pathFollower->follow(
      {{AcceptableDistance{3_s},
        {-0.43_tile, -0.43_tile + rushYAdj, rushH},
        false,
        Path::Parameters{
            1_tile, 0_in_per_s, 0_in_per_s_sq, 76.5_in_per_s_sq}}});
  goalRush->grab();
  wait(goalRushDelay);
  moveTo->reverse(
      {-1.2_tile,
       (GUI::Routines::selectedColor() == MatchColor::Red ? -1.2_tile :
                                                            -0.8_tile) +
           extraY});
  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    turn->toward(0_deg);
  } else {
    turn->toward(180_deg);
  }
  goalRush->release();
  wait(goalRushDelay);
  intake->stop();
  setSortToOpposite();
  clampWhenReady();
  moveTo->reverse(
      {-0.4_tile,
       (GUI::Routines::selectedColor() == MatchColor::Red ? -0.4_tile :
                                                            -1.6_tile) +
           extraY},
      goalClampProfile);
  goalRush->retractArm();
  goalClamp->clamp();
  wait(goalClampDelay);
}

/*
  ___   ___         _
 | _ \ | _ \_  _ __| |_
 |   / |   / || (_-< ' \
 |_|_\ |_|_\\_,_/__/_||_|

*/
void RobotClone::rushRight(const tile_t extraY) {
  intake->setSortOutColor(ColorSensor::Color::None);
  goalRush->extendArm();
  goalRush->release();
  intake->index();
  goalRushWhenReady();
  const tile_t rushYAdj{
      extraY +
      (GUI::Routines::selectedColor() == MatchColor::Blue ? 1_tile : 0_tile)};
  degree_t rushH{id == ID15 ? 105_deg : 110_deg};
  if(GUI::Routines::selectedColor() == MatchColor::Blue) {
    rushH = 180_deg - rushH;
  }
  pathFollower->follow({{AcceptableDistance{3_s},
                         {-0.43_tile, -1.57_tile + rushYAdj, rushH},
                         false,
                         Path::Parameters{(id == ID15 ? 1_tile : 0.5_tile),
                                          0_in_per_s,
                                          0_in_per_s_sq,
                                          76.5_in_per_s_sq}}});
  goalRush->grab();
  wait(goalRushDelay);
  moveTo->reverse(
      {-1.2_tile,
       (GUI::Routines::selectedColor() == MatchColor::Red ? -0.8_tile :
                                                            -1.2_tile) +
           extraY});
  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    turn->toward(90_deg);
  } else {
    turn->toward(180_deg);
  }
  goalRush->release();
  wait(goalRushDelay);
  intake->stop();
  setSortToOpposite();
  clampWhenReady();
  moveTo->reverse(
      {-0.4_tile,
       (GUI::Routines::selectedColor() == MatchColor::Red ? -1.6_tile :
                                                            -0.4_tile) +
           extraY},
      goalClampProfile);
  goalRush->retractArm();
  goalClamp->clamp();
  wait(goalClampDelay);
}

void RobotClone::collectCorner(const bool negative) {
  // Corner Constants
  static const tile_t pressCorner{2.625_tile};
  static const tile_t backupCorner{1.9_tile};
  static const tile_t intakeCorner{3_tile};

  const double yAdj{negative ? 1.0 : -1.0};
  intake->intake();
  moveTo->forward({-intakeCorner, yAdj * intakeCorner},
                  LateralProfile::Parameters{20_in_per_s, 20_in_per_s_sq});
  wait(1.5_s);
  for(int i{0}; i < 0; i++) {
    moveTo->reverse({-backupCorner, yAdj * backupCorner});
    moveTo->forward({-intakeCorner, yAdj * intakeCorner});
  }
  moveTo->reverse({-backupCorner, yAdj * backupCorner});
}

/*
        ___         _
  ___  | __|_ _  __| |
 |___| | _|| ' \/ _` |
       |___|_||_\__,_|

*/
void RobotClone::endOfNegativeRoutines() {
  moveTo->forward({-1_tile, 1_tile});
  intake->intake();

  moveTo->forward({-1_tile, pushDoubleStackY}, doubleStackProfile);
  wait(doubleRingDelay);

  moveTo->forward({-2_tile, 2_tile},
                  LateralProfile::Parameters{singleRingSpeed});
  wait(singleRingDelay);

  collectCorner(true);

  turn->toward({-1_tile, 0_tile});
  pathFollower->follow({{AcceptableDistance{3_s},
                         {-1_tile, 0_tile, 180_deg},
                         false,
                         Path::Parameters{0.5_tile, 40_in_per_s}}});
  intake->stop();
}

/*
    _     ___         _
  _| |_  | __|_ _  __| |
 |_   _| | _|| ' \/ _` |
   |_|   |___|_||_\__,_|

*/
void RobotClone::endOfPositiveRoutines() {
  moveTo->forward({-1_tile, -1_tile});
  intake->intake();
  wait(100_ms);

  moveTo->forward({-2.5_tile, -1_tile});
  wait(singleRingDelay);

  intake->index();

  scheduler.schedule({"Release Goal When Ready",
                      drive->checkIsNear({-2.5_tile, -0.2_tile}, 0.5_ft),
                      [=]() { goalClamp->unclamp(); },
                      1_s});
  moveTo->forward({-2.5_tile, 0.4_tile});
  moveTo->reverse({-1.95_tile, -0.05_tile});
  goalClamp->clamp();
  intake->intake();
  wait(singleRingDelay);
  moveTo->forward({-2_tile, 1.25_tile});
  moveTo->reverse({-2_tile, -1_tile});

  moveTo->forward({-0.4_tile, -2.6_tile}, doubleStackProfile);
  wait(singleRingDelay);

  moveTo->forward({-2_tile, -2_tile},
                  LateralProfile::Parameters{singleRingSpeed});
  wait(singleRingDelay);

  collectCorner(false);

  moveTo->reverse({-2.625_tile, -2.625_tile});
  goalClamp->unclamp();
  pathFollower->follow({{AcceptableDistance{3_s},
                         {-1_tile, 0_tile, 0_deg},
                         false,
                         Path::Parameters{0.5_tile, 40_in_per_s}}});
  intake->stop();
}

void RobotClone::setupRoutine(Pose startingPose) {
  matchTimer.setTime();

  const bool flipped{GUI::Routines::selectedColor() == MatchColor::Blue};
  if(flipped) {
    startingPose.flip();
  }
  Movement::setFlipped(flipped);

  drive->setPose(startingPose);

  setSortToOpposite();

  goalClamp->unclamp();
  goalRush->release();

  if(id == ID15) {
    intake->outtake();
    intake->stop();
  }
  wait(0.1_s);

  drive->setBrakeMode(pros::MotorBrake::brake);
}

void RobotClone::clampWhenReady(const second_t timeout) {
  scheduler.schedule({"Clamp When Ready",
                      [=]() { return goalClamp->hasGoal(); },
                      [=]() {
                        if(goalClamp->isClamped()) {
                          return;
                        }
                        turn->interrupt();
                        moveTo->interrupt();
                        pathFollower->interrupt();
                        goalClamp->clamp();
                      },
                      timeout,
                      Scheduler::doNothing});
}

void RobotClone::goalRushWhenReady(const second_t timeout) {
  scheduler.schedule({"Goal Rush Grab When Ready",
                      [=]() { return goalRush->hasGoal(); },
                      [=]() {
                        if(goalRush->isClamped()) {
                          return;
                        }
                        goalRush->grab();
                        wait(100_ms);
                        turn->interrupt();
                        moveTo->interrupt();
                        pathFollower->interrupt();
                      },
                      timeout,
                      Scheduler::doNothing});
}

void RobotClone::setSortToOpposite() {
  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    intake->setSortOutColor(ColorSensor::Color::Blue);
  } else {
    intake->setSortOutColor(ColorSensor::Color::Red);
  }
}
} // namespace atum
