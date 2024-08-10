#pragma once

#include "../position/tracker.hpp"
#include "../utility/logger.hpp"
#include "../utility/misc.hpp"
#include "api.h"

namespace atum {

class Drive {
  public:
  struct Geometry {
    inch_t driveWidth;
    inch_t wheelCircumference;
    double gearRatio;
  };

  Drive(std::unique_ptr<pros::MotorGroup> iLeft,
        std::unique_ptr<pros::MotorGroup> iRight,
        const Geometry &iGeometry,
        std::unique_ptr<Tracker> iTracker = nullptr,
        std::unique_ptr<Logger> iLogger = nullptr);

  void tank(const double leftVoltage, const double rightVoltage);

  void arcade(const double forwardVoltage, const double turnVoltage);

  void setPosition(const Position &iPosition);

  Position getPosition() const;

  meters_per_second_t getLinearVelocity() const;

  radians_per_second_t getAngularVelocity() const;

  std::pair<double, double> getLRVelocity() const;

  std::pair<double, double> toLRVelocity(const double v, const double w) const;

  void setBrakeMode(const pros::motor_brake_mode_e brakeMode);

  pros::motor_brake_mode_e getBrakeMode() const;

  Geometry getGeometry() const;

  private:
  std::unique_ptr<pros::MotorGroup> left;
  std::unique_ptr<pros::MotorGroup> right;
  const Geometry geometry;
  std::unique_ptr<Tracker> tracker;
  std::unique_ptr<Logger> logger;
};
} // namespace atum