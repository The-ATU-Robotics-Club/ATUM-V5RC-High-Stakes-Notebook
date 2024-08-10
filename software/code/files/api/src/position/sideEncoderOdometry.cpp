#include "sideEncoderOdometry.hpp"

namespace atum {
SideEncoderOdometry::SideEncoderOdometry(std::unique_ptr<Odometer> iL,
                                         std::unique_ptr<Odometer> iR,
                                         std::unique_ptr<Odometer> iS,
                                         std::unique_ptr<IMU> iImu,
                                         std::unique_ptr<Logger> iLogger) :
    Odometry(std::move(iL), std::move(iR), std::move(iImu), std::move(iLogger)),
    s{std::move(iS)} {
  if(!s && logger)
    logger->warn(
        "The side odometer was not provided; using two odometer odometry.");
  if(logger) logger->info("Side Encoder Odometry constructed!");
}

Position SideEncoderOdometry::update() {
  const auto [dl, dr, dh] = getDeltas();
  auto [dxR, dyR] = getRelativeDeltas(dl, dr, dh);
  if(s) {
    inch_t ds{s->traveled()};
    if(dh)
      ds = 2.0 * sin(dh / 2.0) *
           (ds / getValueAs<radian_t>(dh) + s->getFromCenter());
    dxR += ds;
  }
  return integratePosition(dxR, dyR, dh);
}
} // namespace atum