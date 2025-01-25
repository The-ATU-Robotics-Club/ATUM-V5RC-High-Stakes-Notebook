#include "tracker.hpp"

namespace atum {
Tracker::Tracker(const Logger::Level loggerLevel) : logger{loggerLevel} {}

void Tracker::setPose(const Pose &iPose) {
  pose = iPose;
}

Pose Tracker::getPose() {
  if(logger.getLevel() >= Logger::Level::Info) {
    GUI::Map::addPosition(pose, GUI::SeriesColor::Green);
  }
  logger.debug("Tracker pose: " + toString(pose) + ".");
  return pose;
}
} // namespace atum