#include "gps.hpp"

namespace atum {
GPS::GPS(const std::int8_t port,
         const Pose &offset,
         const double iHeadingTrust,
         const double iFullPoseTrust,
         const Logger::Level loggerLevel) :
    headingOffset{offset.h},
    headingTrust{iHeadingTrust},
    fullPoseTrust{iFullPoseTrust},
    logger{loggerLevel} {
  gps = std::make_unique<pros::GPS>(port);
  initializeGPS(offset);
}

GPS::GPS(const Pose &offset,
         const double iHeadingTrust,
         const double iFullPoseTrust,
         const Logger::Level loggerLevel) :
    headingOffset{offset.h},
    headingTrust{iHeadingTrust},
    fullPoseTrust{iFullPoseTrust},
    logger{loggerLevel} {
  const auto gpsSensors{pros::GPS::get_all_devices()};
  if(!gpsSensors.size()) {
    logger.error("GPS not found!");
    gps = std::make_unique<pros::GPS>(errorPort);
    return;
  } else if(gpsSensors.size() > 1) {
    logger.warn("Multiple GPS sensors found! Using first port found.");
  }
  gps = std::make_unique<pros::GPS>(gpsSensors.front().get_port());
  initializeGPS(offset);
}

void GPS::setPose(const Pose &pose) {
  const double x{getValueAs<meter_t>(pose.x)};
  const double y{getValueAs<meter_t>(pose.y)};
  const double h{
      getValueAs<degree_t>(constrain180(pose.h + headingOffset) + 180_deg)};
  gps->set_position(x, y, h);
}

Pose GPS::getPose() {
  const meter_t x{gps->get_position_x()};
  const meter_t y{gps->get_position_y()};
  const degree_t reading{gps->get_yaw()};
  const degree_t h{reading - headingOffset};
  if(logger.getLevel() >= Logger::Level::Info) {
    GUI::Map::addPosition({x, y}, GUI::SeriesColor::Yellow);
  }
  logger.debug("GPS pose: " + toString({x, y, h}) + ".");
  return {x, y, h};
}

void GPS::resetTracker(Tracker *tracker) {
  if(!check() || gps->get_error() >= maxError) {
    return;
  }
  const Pose currentPose{getPose()};
  Pose newPose{fullPoseTrust * currentPose +
               (1.0 - fullPoseTrust) * tracker->getPose()};
  newPose.h = getHeading(currentPose.h);
  // Don't reset anything but x, y, and h.
  newPose.v = currentPose.v;
  newPose.a = currentPose.a;
  newPose.omega = currentPose.omega;
  newPose.alpha = currentPose.alpha;
  newPose.t = currentPose.t;
  tracker->setPose(newPose);
}

bool GPS::check() {
  const bool installed{gps->is_installed()};
  if(!installed) {
    logger.error("GPS on port " + std::to_string(gps->get_port()) +
                 " is not installed!");
  }
  return installed;
}

degree_t GPS::getHeading(const degree_t otherHeading) {
  if(!check()) {
    return otherHeading;
  }
  const degree_t reading{gps->get_yaw()};
  const degree_t gpsHeading{reading - headingOffset};
  return headingTrust * gpsHeading + (1.0 - headingTrust) * otherHeading;
}

void GPS::initializeGPS(const UnwrappedPose &offset) {
  gps->set_data_rate(5);
  gps->set_offset(offset.x, offset.y);
  check();
}
} // namespace atum