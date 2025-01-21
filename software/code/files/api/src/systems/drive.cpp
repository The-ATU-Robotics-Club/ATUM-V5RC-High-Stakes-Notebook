#include "drive.hpp"

namespace atum {
Drive::Drive(std::unique_ptr<Motor> iLeft,
             std::unique_ptr<Motor> iRight,
             std::unique_ptr<Tracker> iTracker,
             const Geometry &iGeometry,
             const Logger::Level loggerLevel) :
    left{std::move(iLeft)},
    right{std::move(iRight)},
    tracker{std::move(iTracker)},
    geometry{iGeometry},
    logger{loggerLevel} {
  if(!left) {
    logger.error("The left side drive motors were not provided!");
  }
  if(!right) {
    logger.error("The right side drive motors were not provided!");
  }
  if(!tracker) {
    logger.error("No tracker provided for the drive!");
    return;
  }
  logger.info("Drive constructed!");
}

void Drive::tank(const double leftVoltage, const double rightVoltage) {
  if(std::abs(leftVoltage) > Motor::maxVoltage ||
     std::abs(rightVoltage) > Motor::maxVoltage) {
    logger.warn("Input voltage exceeds motor range (>12V).");
  }
  if(!leftVoltage && !rightVoltage) {
    brake();
  } else {
    left->moveVoltage(leftVoltage);
    right->moveVoltage(rightVoltage);
  }
}

void Drive::arcade(const double forwardVoltage, const double turnVoltage) {
  const double left{std::clamp(
      forwardVoltage + turnVoltage, -Motor::maxVoltage, Motor::maxVoltage)};
  const double right{std::clamp(
      forwardVoltage - turnVoltage, -Motor::maxVoltage, Motor::maxVoltage)};
  tank(left, right);
}

void Drive::brake() {
  left->brake();
  right->brake();
}

void Drive::setPose(const Pose &iPose) {
  tracker->setPose(iPose);
}

Pose Drive::getPose() const {
  return tracker->getPose();
}

std::pair<revolutions_per_minute_t, revolutions_per_minute_t>
    Drive::getLRVelocity() const {
  return std::make_pair(left->getVelocity(), right->getVelocity());
}

void Drive::setBrakeMode(const pros::v5::MotorBrake brakeMode) {
  left->setBrakeMode(brakeMode);
  right->setBrakeMode(brakeMode);
}

pros::v5::MotorBrake Drive::getBrakeMode() const {
  return left->getBrakeMode();
}

Drive::Geometry Drive::getGeometry() const {
  return geometry;
}

revolutions_per_minute_t Drive::getMaxRPM() const {
  return left->getMaxRPM();
}

meters_per_second_t Drive::getMaxVelocity() const {
  const double maxRPM{getValueAs<revolutions_per_minute_t>(left->getMaxRPM())};
  return maxRPM * geometry.circum / 1_min;
}

Condition Drive::checkIsNear(const Pose pose, const meter_t threshold) {
  return [=]() { return distance(getPose(), pose) <= threshold; };
}
} // namespace atum