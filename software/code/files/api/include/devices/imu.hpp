#pragma once

#include "../utility/logger.hpp"
#include "../utility/units.hpp"
#include "pros/apix.h"

namespace atum {
class IMU {
  public:
  IMU(std::initializer_list<std::uint8_t> ports,
      const bool iReversed = -1,
      const double iImuTrust = 1.0,
      std::unique_ptr<Logger> iLogger = nullptr);

  void setHeading(degree_t heading);

  degree_t getHeading() const;

  degree_t getTraveled();

  degree_t combine(const degree_t other);

  protected:
  std::vector<std::unique_ptr<pros::IMU>> imus;
  const bool reversed;
  const double imuTrust;
  std::unique_ptr<Logger> logger;
  degree_t previous{0_deg};
};
} // namespace atum