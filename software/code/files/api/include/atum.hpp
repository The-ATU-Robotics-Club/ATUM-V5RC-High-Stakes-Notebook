/**
 * @file atum.hpp
 * @brief Lists the header files of the ATUM API to make it easier to include
 * all-at-once.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "controllers/controller.hpp"
#include "controllers/pid.hpp"
#include "controllers/slewRate.hpp"
#include "controllers/tbh.hpp"
#include "depend/units.h"
#include "devices/adi.hpp"
#include "devices/colorSensor.hpp"
#include "devices/distanceSensor.hpp"
#include "devices/imu.hpp"
#include "devices/limitSwitch.hpp"
#include "devices/lineTracker.hpp"
#include "devices/motor.hpp"
#include "devices/odometer.hpp"
#include "devices/piston.hpp"
#include "devices/potentiometer.hpp"
#include "devices/rotationSensor.hpp"
#include "gui/graph.hpp"
#include "gui/log.hpp"
#include "gui/manager.hpp"
#include "gui/map.hpp"
#include "gui/routines.hpp"
#include "gui/screen.hpp"
#include "motion/kinematics.hpp"
#include "motion/motionProfile.hpp"
#include "motion/path.hpp"
#include "motion/pathFollower.hpp"
#include "motion/profileFollower.hpp"
#include "motion/turn.hpp"
#include "pose/odometry.hpp"
#include "pose/tracker.hpp"
#include "systems/drive.hpp"
#include "systems/remote.hpp"
#include "systems/robot.hpp"
#include "systems/stateMachine.hpp"
#include "time/schedule.hpp"
#include "time/task.hpp"
#include "time/time.hpp"
#include "time/timer.hpp"
#include "utility/acceptable.hpp"
#include "utility/logger.hpp"
#include "utility/misc.hpp"
#include "utility/units.hpp"
