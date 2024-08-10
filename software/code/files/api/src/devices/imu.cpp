#include "imu.hpp"

namespace atum {
IMU::IMU(std::initializer_list<std::uint8_t> ports,
         const bool iReversed,
         const double iImuTrust,
         std::unique_ptr<Logger> iLogger) :
    reversed{iReversed}, imuTrust{iImuTrust}, logger(std::move(iLogger)) {
  if((imuTrust < 0.0 || imuTrust > 1.0) && logger)
    logger->error("IMU trust should be between 0 and 1 inclusive.");
  for(std::uint8_t port : ports) {
    imus.push_back(std::make_unique<pros::IMU>(port));
    if(pros::c::registry_get_plugged_type(port - 1) !=
       pros::c::v5_device_e_t::E_DEVICE_IMU) {
      if(logger)
        logger->error("IMU at port " + std::to_string(port) +
                      " could not be initialized!");
      imus.erase(imus.end());
    }
  }
  for(auto &imu : imus) imu->set_data_rate(5);
  if(logger) logger->info("IMU is constructed!");
  while(imus.size() && imus.back()->is_calibrating()) pros::delay(10);
  if(logger) logger->info("IMU is calibrated!");
}

void IMU::setHeading(degree_t heading) {
  if(reversed) heading *= -1;
  for(auto &imu : imus) imu->set_rotation(getValueAs<degree_t>(heading));
  previous = heading;
}

degree_t IMU::getHeading() const {
  std::vector<degree_t> readings;
  for(auto &imu : imus) readings.push_back(degree_t{imu->get_rotation()});
  degree_t heading{average(readings)};
  if(reversed) heading *= -1;
  if(logger) logger->debug("IMU Rotation: " + to_string(heading));
  return heading;
}

degree_t IMU::getTraveled() {
  const degree_t current{getHeading()};
  const degree_t dh{current - previous};
  previous = current;
  return dh;
}

degree_t IMU::combine(const degree_t other) {
  return imuTrust * getTraveled() + (1.0 - imuTrust) * other;
}
} // namespace atum