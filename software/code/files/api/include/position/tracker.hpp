#pragma once

#include "../time/timer.hpp"
#include "../utility/logger.hpp"
#include "position.hpp"

namespace atum {
class Tracker {
  public:
  Tracker(std::unique_ptr<Logger> iLogger = nullptr);

  virtual Position update() = 0;

  virtual void setPosition(const Position &iPosition);

  virtual Position getPosition();

  protected:
  std::pair<meters_per_second_t, radians_per_second_t>
      getVW(const inch_t distance, const radian_t dh);

  std::unique_ptr<Logger> logger;
  Position position{0_tile, 0_tile, 0_deg};
  pros::Mutex positionMutex;

  private:
  Timer timer;
};
} // namespace atum