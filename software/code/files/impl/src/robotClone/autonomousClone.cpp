#include "atum/pose/pose.hpp"
#include "atum/systems/robot.hpp"
#include "robotClone.hpp"


namespace atum {
// Max drive velocity: 76.5 in. / s.
// Max drive acceleration: 76.5 in. / s^2.
ROUTINE_DEFINITIONS_FOR(RobotClone) {
  /*
    ___ _   _ _ _
   / __| |_(_) | |___
   \__ \ / / | | (_-<
   |___/_\_\_|_|_/__/
  */

  START_ROUTINE("Skills") 

  if(id == ID15) {
  setupRoutine({-5.3_ft, 5.3_ft, -45_deg});

  intake->intake();

  pathFollower->follow({{{-5.6_ft, 5.6_ft, -45_deg}, false, Path::Parameters{}}});
  

  intake->stop();

  // ring 1

  pathFollower->follow(
      {{{-4.5_ft, 4.5_ft, 135_deg}, true, Path::Parameters{}}});

  turn->awayFrom(90_deg);

  pathFollower->follow({{{-2_ft, 4_ft, 90_deg}, true, Path::Parameters{}}});

  goalClamp->clamp();

  goalClamp->clamp();

  turn->toward(-90_deg);

  // ring 2
  intake->intake();

  pathFollower->follow({{{-4_ft, 4_ft, -90_deg}, false, Path::Parameters{}}});

  intake->stop();

  turn->toward(135_deg);

  pathFollower->follow({{{-2_ft, 2_ft, 135_deg}, false, Path::Parameters{}}});

  // ring 3
  intake->intake();

  intake->stop();

  pathFollower->follow(
      {{{-0.25_ft, 0.25_ft, 90_deg}, false, Path::Parameters{3, 30_in_per_s}}});

  // ring 4
  intake->intake();

  intake->stop();

  pathFollower->follow({{{0.25_ft, 0.25_ft, 90_deg}, false, Path::Parameters{4, 10_in_per_s}}});

  // ring 5
  intake->intake();

  intake->stop();

  turn->toward(-45_deg);

  pathFollower->follow({{{-6_ft, 6_ft, -45_deg}, true, Path::Parameters{}}});

  goalClamp->unclamp();

  turn->toward(90_deg);

  pathFollower->follow({{{0_ft, 5_ft, 90_deg}, false, Path::Parameters{}}});

  // ring 6
  intake->intake();

  intake->stop();

  turn->toward(180_deg);

  pathFollower->follow({{{0_ft, 4_ft, 180_deg}, false, Path::Parameters{}}});

  // ring 7
  intake->intake();

  intake->stop();

  turn->toward(0_deg);

  pathFollower->follow({{{0_ft, 6_ft, 0_deg}, false, Path::Parameters{}}});

  // LB things

  turn->toward(90_deg);

  pathFollower->follow({{{2_ft, 5_ft, 90_deg}, false, Path::Parameters{}}});

  //ring 8

  intake->intake();

  intake->stop();

  pathFollower->follow({{{0_ft, 5_ft, -90_deg}, true, Path::Parameters{}}});

  turn->toward(0_deg);

  // LB things
  
  turn->toward(135_deg);

  pathFollower->follow({{{2_ft, 2_ft, 135_deg}, false, Path::Parameters{}}});

  goalClamp->clamp();

  turn->toward(0_deg);

  intake->intake();

  pathFollower->follow({{{2_ft, 4_ft, 0_deg}, false, Path::Parameters{}}});

  turn->toward(90_deg);

  pathFollower->follow({{{4_ft, 4_ft, 90_deg}, false, Path::Parameters{}}});

  intake->stop();

  turn->toward(45_deg);

  intake->intake();

  pathFollower->follow({{{5.8_ft, 5.8_ft, 45_deg}, false, Path::Parameters{}}});

  pathFollower->follow({{{5_ft, 5_ft, -135_deg}, true, Path::Parameters{}}});

  pathFollower->follow({{{5.8_ft, 5.8_ft, 45_deg}, false, Path::Parameters{}}});

  intake->stop();

  pathFollower->follow({{{2_ft, 2_ft, -135_deg}, false, Path::Parameters{}}});

  turn->toward(0_deg);

  pathFollower->follow({{{2_ft, 5.5_ft, 0_deg}, false, Path::Parameters{}}});

  
  } else if(id == ID24) {

    intake->load();
  pathFollower->follow({{{-5.5_ft, 0_ft, 90_deg}, false, Path::Parameters{}}});
  // LADY BROWN
  intake->intake();
  pathFollower->follow({{{-4_ft, -4_ft, 155_deg}, false, Path::Parameters{}}});
  turn->awayFrom(90_deg);
  pathFollower->follow({{{-2_ft, -4_ft, 90_deg}, true, Path::Parameters{}}});
  // Clamp mogo
  pathFollower->follow({{{-6_ft, -6_ft, -135_deg}, false, Path::Parameters{}}});
  pathFollower->follow(
      {{{-0.5_ft, -0.5_ft, 90_deg}, false, Path::Parameters{}}});
  pathFollower->follow({{{0.5_ft, 0.5_ft, 90_deg}, false, Path::Parameters{}}});
  pathFollower->follow({{{-6_ft, -6_ft, -135_deg}, true, Path::Parameters{}}});
  // Unclamp mogo
  pathFollower->follow({{{0_ft, -5_ft, 90_deg}, false, Path::Parameters{}}});
  pathFollower->follow({{{0_ft, -4_ft, 45_deg}, false, Path::Parameters{}}});
  pathFollower->follow({{{0_ft, -5.5_ft, 180_deg}, false, Path::Parameters{}}});
  // LADY BROWN 2 RINGS
  pathFollower->follow({{{.5_ft, -5_ft, 90_deg}, false, Path::Parameters{}}});
  pathFollower->follow({{{0_ft, -5.5_ft, 180_deg}, false, Path::Parameters{}}});
  // LADY BROWN 1 MORE RING
  pathFollower->follow({{{2_ft, -2_ft, 45_deg}, true, Path::Parameters{}}});
  // CLAMP MOGO
  pathFollower->follow({{{2_ft, -4_ft, 180_deg}, false, Path::Parameters{}}});
  // INTAKE ONLY RED RING
  pathFollower->follow({{{4_ft, -4_ft, 90_deg}, false, Path::Parameters{}}});
  // INTAKE ONLY RED RING
  pathFollower->follow({{{6_ft, -6_ft, 135_deg}, false, Path::Parameters{}}});
  // INTAKE ONLY RED RING
  pathFollower->follow({{{2_ft, -4_ft, -45_deg}, false, Path::Parameters{}}});
  pathFollower->follow({{{4.5_ft, -4_ft, 90_deg}, false, Path::Parameters{}}});
  pathFollower->follow({{{6_ft, -6_ft, 135_deg}, false, Path::Parameters{}}});
  turn->toward(-45_deg);
  // RELEASE GOAL CLAMP

  // RAISE HANG MECH
  pathFollower->follow({{{.5_ft, -.5_ft, -45_deg}, true, Path::Parameters{}}});
  // HANG
    
  }

  END_ROUTINE



  START_ROUTINE("Full Test")
  setupRoutine({-5_ft, -3_ft, 180_deg});

  pathFollower->follow(
      {{{-1_ft, -4.625_ft, 90_deg}, false, Path::Parameters{2.8, 25_in_per_s}}},
      "Test Curve");
  turn->toward(-15_deg);
  pathFollower->follow(
      {{{0_tile, 0_tile, 45_deg}, false, Path::Parameters{3.5}}});
  turn->awayFrom(135_deg);
  pathFollower->follow(
      {{{1.5_tile, -2.5_tile, 0_deg}, true, Path::Parameters{4}},
       {{1.5_tile, -1.5_tile, 0_deg}, false}});
  turn->toward({0_in, 0_in});

  END_ROUTINE

  START_ROUTINE("Turning Test")
  setupRoutine({0_in, 0_in, 0_deg});

  turn->toward(90_deg);    // Right
  turn->toward(180_deg);   // Right
  turn->toward(-90_deg);   // Right
  turn->awayFrom(0_deg);   // Left
  turn->awayFrom(180_deg); // About
  turn->toward(90_deg);    // Right
  turn->toward(-90_deg);   // About
  turn->awayFrom(0_deg);   // Left
  turn->awayFrom(180_deg); // About
  turn->toward(45_deg);
  turn->toward(90_deg);
  turn->toward(135_deg);
  turn->toward(180_deg);
  turn->toward(225_deg);
  turn->toward(270_deg);
  turn->toward(315_deg);
  turn->toward(360_deg);
  END_ROUTINE









  /*
    __  __ _    _      __ _        _     ___           _    _
   |  \/  (_)__| |___ / _(_)_ _ __| |_  |   \ ___ _  _| |__| |___
   | |\/| | / _` |___|  _| | '_(_-<  _| | |) / _ \ || | '_ \ / -_)
   |_|  |_|_\__,_|   |_| |_|_| /__/\__| |___/\___/\_,_|_.__/_\___|
  */
  START_ROUTINE("Mid-first Double Goal")
  setupRoutine({-2_tile - 4_in, -0.5_tile, 90_deg});
  intake->index();
  pathFollower->follow(
      {{{-1.375_tile, -0.55_tile, 90_deg, 76.5_in_per_s},
        false,
        Path::Parameters{1, 76.5_in_per_s, 76.5_in_per_s_sq}},
       {{-0.5_tile, -0.5_tile, 45_deg},
        false,
        Path::Parameters{0.75, 76.5_in_per_s, 76.5_in_per_s_sq}}});
  wait(0.1_s); // Time for goal rush mech to clamp.
  pathFollower->follow(
      {{{-1.1_tile, -1.1_tile, 45_deg},
        true,
        Path::Parameters{1, 76.5_in_per_s, 76.5_in_per_s_sq}}});
  turn->toward({0_tile, -2_tile});
  pathFollower->follow(
      {{{-0.4_tile, -1.6_tile, 135_deg},
        false,
        Path::Parameters{1, 76.5_in_per_s, 76.5_in_per_s_sq}}});
  wait(0.1_s); // Time for goal rush mech to clamp.
  pathFollower->follow(
      {{{-1.1_tile, -1.1_tile, 135_deg},
        true,
        Path::Parameters{1, 76.5_in_per_s, 76.5_in_per_s_sq}}});
  turn->toward(-135_deg);
  clampWhenReady();
  pathFollower->follow(
      {{{-0.9_tile, -0.9_tile, -135_deg}, true, Path::Parameters{0.25}}});

  goalClamp->clamp();
  wait(0.1_s); // Time for goal clamp mech to clamp.
  intake->intake();
  wait(0.25_s);
  pathFollower->follow(
      {{{-1_tile, -1_tile, -135_deg}, false, Path::Parameters{0.25}}});
  endDoubleGoalRush();
  END_ROUTINE

  /*
      ___ _    _          __ _        _     ___           _    _
     / __(_)__| |___ ___ / _(_)_ _ __| |_  |   \ ___ _  _| |__| |___
     \__ \ / _` / -_)___|  _| | '_(_-<  _| | |) / _ \ || | '_ \ / -_)
     |___/_\__,_\___|   |_| |_|_| /__/\__| |___/\___/\_,_|_.__/_\___|
  */
  START_ROUTINE("Side-first Double Goal")
  setupRoutine({-2_tile - 4_in, -1.5_tile, 90_deg});
  intake->index();

  // Grab side goal.
  pathFollower->follow(
      {{{-0.5_tile, -1.5_tile, 135_deg}, false, Path::Parameters{1}}});
  wait(0.1_s); // Time for goal rush mech to clamp.
  pathFollower->follow(
      {{{-1.1_tile, -0.9_tile, 135_deg}, true, Path::Parameters{1}}});
  turn->toward({0_tile, 0_tile});
  pathFollower->follow(
      {{{-0.5_tile, -0.5_tile, 45_deg}, false, Path::Parameters{1}}});
  wait(0.1_s); // Time for goal rush mech to clamp.
  pathFollower->follow(
      {{{-1_tile, -1_tile, 45_deg}, true, Path::Parameters{1}}});
  turn->toward(-45_deg);
  clampWhenReady();
  pathFollower->follow(
      {{{-0.9_tile, -1.1_tile, -45_deg}, true, Path::Parameters{0.25}}});
  goalClamp->clamp();
  wait(0.1_s); // Time for goal clamp mech to clamp.
  intake->intake();
  wait(0.25_s);
  pathFollower->follow(
      {{{-1_tile, -1_tile, -45_deg}, false, Path::Parameters{0.25}}});
  endDoubleGoalRush();
  END_ROUTINE
}

void RobotClone::endDoubleGoalRush() {
  // Score first stack of rings.
  turn->toward({-1_tile, -2.5_tile});
  pathFollower->follow({{{-1_tile,
                          -2.5_tile,
                          angle(drive->getPose(), {-1_tile, -2.5_tile}, true)},
                         false,
                         Path::Parameters{2, 35_in_per_s}}});

  // Score second stack of rings.
  turn->toward({-2_tile, -2_tile});
  pathFollower->follow(
      {{{-2_tile, -2_tile, angle(drive->getPose(), {-2_tile, -2_tile}, true)},
        false,
        Path::Parameters{0.15}}});

  // Score the rings in the corner.
  turn->toward(-135_deg);
  intake->resetCount();
  while(matchTimer.timeElapsed() <= 23_s) {
    turn->toward(-135_deg);
    pathFollower->follow(
        {{{-2.575_tile, -2.575_tile, -135_deg},
          false,
          Path::Parameters{0.15, 15_in_per_s, 15_in_per_s_sq}}});
    pathFollower->follow(
        {{{-2_tile, -2_tile, -135_deg}, true, Path::Parameters{0.25}}});
    wait(0.15_s);
  }

  // Place goal in corner and go touch ladder.
  turn->toward(45_deg);
  pathFollower->follow(
      {{{-2.5_tile, -2.5_tile, 45_deg}, true, Path::Parameters{0.15}}});
  intake->stop();
  goalClamp->unclamp();
  pathFollower->follow({{{-0.15_tile, -0.95_tile, 30_deg},
                         false,
                         Path::Parameters{2.5, 50_in_per_s}}});
}

void RobotClone::setupRoutine(Pose startingPose) {
  matchTimer.setTime();

  const inch_t extension{6.5_in};
  startingPose.x += extension;
  const bool flipped{GUI::Routines::selectedColor() == MatchColor::Blue};
  if(flipped) {
    startingPose.flip();
  }
  pathFollower->setFlipped(flipped);
  turn->setFlipped(flipped);

  drive->setPose(startingPose);
  gps->setPose(startingPose);

  if(GUI::Routines::selectedColor() == MatchColor::Red) {
    intake->setSortOutColor(ColorSensor::Color::Blue);
  } else {
    intake->setSortOutColor(ColorSensor::Color::Red);
  }

  goalClamp->unclamp();
}

Schedule RobotClone::clampWhenReady(const second_t timeout) {
  return Schedule{Schedule::Item{"Clamp When Ready",
                                 [=]() {
                                   pathFollower->interrupt();
                                   turn->interrupt();
                                   return goalClamp->hasGoal();
                                 },
                                 [=]() { goalClamp->clamp(); },
                                 timeout}};
}
} // namespace atum
