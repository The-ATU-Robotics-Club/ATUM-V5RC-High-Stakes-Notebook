#pragma once

#include "../devices/imu.hpp"
#include "../devices/odometer.hpp"
#include "../time/task.hpp"
#include "../utility/units.hpp"
#include "tracker.hpp"

namespace atum {
class Odometry : public Tracker, public Task {
  public:
  Odometry(std::unique_ptr<Odometer> iL,
           std::unique_ptr<Odometer> iR,
           std::unique_ptr<IMU> iImu,
           std::unique_ptr<Logger> iLogger = nullptr);

  virtual Position update() override;

  protected:
  std::tuple<inch_t, inch_t, radian_t> getDeltas();

  std::pair<inch_t, inch_t>
      getRelativeDeltas(const inch_t dl, const inch_t dr, const radian_t dh);

  Position integratePosition(inch_t dx, inch_t dy, radian_t dh);

  std::unique_ptr<Odometer> l;
  std::unique_ptr<Odometer> r;
  std::unique_ptr<IMU> imu;

  private:
  void taskFn1() override;
};
} // namespace atum