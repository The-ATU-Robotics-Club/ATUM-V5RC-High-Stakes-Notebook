#include "odometry.hpp"

namespace atum {
Odometry::Odometry(std::unique_ptr<Odometer> iL,
                   std::unique_ptr<Odometer> iR,
                   std::unique_ptr<IMU> iImu,
                   std::unique_ptr<Logger> iLogger) :
    Tracker(std::move(iLogger)),
    Task({"Odometry", TASK_PRIORITY_MAX}),
    l{std::move(iL)},
    r{std::move(iR)},
    imu{std::move(iImu)} {
  if(!r && logger) logger->error("The right odometer must be provided.");
  if(!l && !imu && logger)
    logger->error(
        "At least either the left odometer or an IMU must be provided.");
  if(logger) logger->info("Odometry constructed!");
}

Position Odometry::update() {
  const auto [dl, dr, dh] = getDeltas();
  const auto [dxR, dyR] = getRelativeDeltas(dl, dr, dh);
  return integratePosition(dxR, dyR, dh);
}

std::tuple<inch_t, inch_t, radian_t> Odometry::getDeltas() {
  const inch_t dr{r->traveled()};
  inch_t dl{dr};
  radian_t dh;
  if(l) {
    dl = l->traveled();
    dh = radian_t{getValueAs<meter_t>(dl - dr) /
                  getValueAs<meter_t>(l->getFromCenter() + r->getFromCenter())};
    if(imu) dh = imu->combine(dh);
  } else
    dh = imu->getTraveled();
  return std::make_tuple(dl, dr, dh);
}

std::pair<inch_t, inch_t> Odometry::getRelativeDeltas(const inch_t dl,
                                                      const inch_t dr,
                                                      const radian_t dh) {
  inch_t dx{0.0_in};
  inch_t dy{0.0_in};
  if(!dh) {
    const inch_t dd = (dl + dr) / 2.0;
    dx = sin(dh) * dd;
    dy = cos(dh) * dd;
  } else if(!l)
    dy = 2.0 * sin(dh / 2.0) *
         (dr / getValueAs<radian_t>(dh) + r->getFromCenter());
  else {
    const inch_t radius{(l->getFromCenter() + r->getFromCenter()) / 2.0};
    dy = radius * (dl + dr) / (dr - dl);
  }
  return std::make_pair(dx, dy);
}

Position Odometry::integratePosition(inch_t dx, inch_t dy, radian_t dh) {
  if(!std::isfinite(getValueAs<inch_t>(dx))) dx = 0_in;
  if(!std::isfinite(getValueAs<inch_t>(dy))) dy = 0_in;
  if(!std::isfinite(getValueAs<radian_t>(dh))) dh = 0_rad;
  Position currentPosition{getPosition()};
  Position previousPosition{currentPosition};
  const radian_t hAvg{currentPosition.h + 0.5 * dh};
  currentPosition.x += cos(hAvg) * dx + sin(hAvg) * dy;
  currentPosition.y += -sin(hAvg) * dx + cos(hAvg) * dy;
  currentPosition.h += dh;
  auto [v, w] = getVW(distance(previousPosition, currentPosition), dh);
  currentPosition.v = v;
  currentPosition.w = w;
  setPosition(currentPosition);
  if(logger) logger->debug("Odometry Reading: " + toString(currentPosition));
  return currentPosition;
}

void Odometry::taskFn1() {
  while(true) {
    update();
    wait(10_ms);
  }
}
} // namespace atum