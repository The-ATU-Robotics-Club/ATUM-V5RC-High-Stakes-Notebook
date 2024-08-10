#pragma once

#include "../devices/odometer.hpp"
#include "../time/task.hpp"
#include "../utility/units.hpp"
#include "odometry.hpp"

namespace atum {
class SideEncoderOdometry : public Odometry {
  public:
  SideEncoderOdometry(std::unique_ptr<Odometer> iL,
                      std::unique_ptr<Odometer> iR,
                      std::unique_ptr<Odometer> iS,
                      std::unique_ptr<IMU> iImu,
                      std::unique_ptr<Logger> iLogger = nullptr);

  Position update() override;

  private:
  std::unique_ptr<Odometer> s;
};
} // namespace atum