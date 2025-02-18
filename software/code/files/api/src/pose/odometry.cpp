#include "odometry.hpp"

namespace atum {
Odometry::Odometry(std::unique_ptr<Odometer> iForward,
                   std::unique_ptr<Odometer> iSide,
                   std::unique_ptr<IMU> iImu,
                   Drive *iDrive,
                   Logger::Level loggerLevel) :
    Tracker(loggerLevel),
    Task(this, loggerLevel),
    forward{std::move(iForward)},
    side{std::move(iSide)},
    imu{std::move(iImu)},
    drive{iDrive} {
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
  if(drive) {
    inch_t dyRDrive {drive->traveled()};
    if(dh) {
      dyRDrive = 2.0 * sin(dh / 2.0) * (dyRDrive / dhScalar);
    }
    // dyR = (dyR + dyRDrive) / 2.0;
  }
  return integratePose(dxR, dyR, dh);
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
  const degree_t hAvg{currentPose.h + dh / 2.0};
  currentPose.x += cos(hAvg) * dx + sin(hAvg) * dy;
  currentPose.y += -sin(hAvg) * dx + cos(hAvg) * dy;
  currentPose.h += dh;
  const second_t dt{timer.getDT()};
  currentPose.v = dy / dt;
  currentPose.omega = dh / dt;
  setPose(currentPose);
  return getPose(); // Use getPose() for logging purposes.f
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