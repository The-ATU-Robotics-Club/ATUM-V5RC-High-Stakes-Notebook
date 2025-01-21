#include "odometry.hpp"

namespace atum {
Odometry::Odometry(std::unique_ptr<Odometer> iForward,
                   std::unique_ptr<Odometer> iSide,
                   std::unique_ptr<IMU> iImu,
                   Logger::Level loggerLevel) :
    Tracker(loggerLevel),
    Task(this, loggerLevel),
    forward{std::move(iForward)},
    side{std::move(iSide)},
    imu{std::move(iImu)} {
  if(!forward) {
    logger.error("The forward odometer must be provided.");
  }
  if(!side) {
    logger.error("The side odometer was not provided.");
  }
  if(!imu) {
    logger.error("An IMU must be provided.");
  }
  logger.info("Odometry constructed!");
}

Pose Odometry::update() {
  inch_t dxR{side->traveled()};
  inch_t dyR{forward->traveled()};
  const radian_t dh{imu->getTraveled()};
  const double dhScalar{getValueAs<radian_t>(dh)};
  // Initial values are found by their Taylor series evaluated at dh = 0.
  scalar_t sinDHOverDH{1.0};
  scalar_t cosDHMinusOneOverDH{0.0};
  if(dh) {
    // Arc length estimation using the distance from the center to the
    // odometers.
    dxR = 2.0 * sin(dh / 2.0) * (dxR / dhScalar + side->getFromCenter());
    dyR = 2.0 * sin(dh / 2.0) * (dyR / dhScalar + forward->getFromCenter());
    sinDHOverDH = sin(dh) / dhScalar;
    cosDHMinusOneOverDH = (cos(dh) - 1.0) / dhScalar;
  }
  // Accounting for angular velocity.
  const inch_t dxRAdj{sinDHOverDH * dxR + cosDHMinusOneOverDH * dyR};
  const inch_t dyRAdj{-cosDHMinusOneOverDH * dxR + sinDHOverDH * dyR};
  return integratePose(dxRAdj, dyRAdj, dh);
}

Pose Odometry::integratePose(inch_t dx, inch_t dy, radian_t dh) {
  if(!std::isfinite(getValueAs<inch_t>(dx)) ||
     !std::isfinite(getValueAs<inch_t>(dy)) ||
     !std::isfinite(getValueAs<radian_t>(dh))) {
    logger.warn("Invalid values read from odometers.");
    dx = 0_in;
    dy = 0_in;
    dh = 0_rad;
  }
  Pose currentPose{getPose()};
  currentPose.x += cos(currentPose.h) * dx + sin(currentPose.h) * dy;
  currentPose.y += -sin(currentPose.h) * dx + cos(currentPose.h) * dy;
  currentPose.h += dh;
  const second_t dt{timer.timeElapsed()};
  currentPose.v = dy / dt;
  currentPose.omega = dh / dt;
  timer.setTime();
  setPose(currentPose);
  return getPose(); // Use getPose() for logging purposes.
}

TASK_DEFINITIONS_FOR(Odometry) {
  START_TASK("Odometry Loop", TASK_PRIORITY_MAX)
  while(true) {
    update();
    wait(10_ms);
  }
  END_TASK
}
} // namespace atum