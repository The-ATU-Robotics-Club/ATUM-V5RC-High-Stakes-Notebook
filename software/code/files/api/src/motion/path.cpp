#include "path.hpp"

namespace atum {
Path::Parameters::Parameters(const double iCurviness,
                             const meters_per_second_t iMaxV,
                             const meters_per_second_squared_t iMaxA,
                             const meter_t iTrack,
                             const meter_t iSpacing,
                             const meter_t iMaxSpacingError,
                             const bool iUsePosition,
                             const double iBinarySearchScaling) :
    curviness{iCurviness},
    maxV{iMaxV},
    maxA{iMaxA},
    track{iTrack},
    spacing{iSpacing},
    maxSpacingError{iMaxSpacingError},
    usePosition{iUsePosition},
    binarySearchScaling{iBinarySearchScaling} {}

Path::Parameters::Parameters(const meters_per_second_t iMaxV,
                             const double iCurviness,
                             const meters_per_second_squared_t iMaxA,
                             const meter_t iTrack,
                             const meter_t iSpacing,
                             const meter_t iMaxSpacingError,
                             const bool iUsePosition,
                             const double iBinarySearchScaling) :
    curviness{iCurviness},
    maxV{iMaxV},
    maxA{iMaxA},
    track{iTrack},
    spacing{iSpacing},
    maxSpacingError{iMaxSpacingError},
    usePosition{iUsePosition},
    binarySearchScaling{iBinarySearchScaling} {}

Path::Parameters::Parameters(const Path::Parameters &other) :
    spacing{other.spacing}, binarySearchScaling{other.binarySearchScaling} {
  curviness = other.curviness;
  maxV = other.maxV;
  maxA = other.maxA;
  track = other.track;
}

Path::Parameters &Path::Parameters::operator=(const Path::Parameters &other) {
  if(this == &other) {
    return *this;
  }
  if(other.curviness) {
    curviness = other.curviness;
  }
  if(other.maxV) {
    maxV = other.maxV;
  }
  if(other.maxA) {
    maxA = other.maxA;
  }
  return *this;
}

Path::Path(const std::pair<Pose, Pose> &waypoints,
           const std::optional<Parameters> &specialParams,
           const Logger::Level loggerLevel) :
    start{waypoints.first},
    end{waypoints.second},
    params{defaultParams},
    logger{loggerLevel} {
  if(specialParams.has_value()) {
    params = specialParams.value();
  }
  const degree_t startH{90_deg - start.h};
  startDirection =
      params.curviness * Pose{1_m * cos(startH), 1_m * sin(startH)};
  const degree_t endH{90_deg - end.h};
  endDirection = params.curviness * Pose{1_m * cos(endH), 1_m * sin(endH)};
  generate();
  logger.debug("Path has been generated!");
}

Pose Path::getPose(const Pose &state) {
  Pose pose{getTimed()};
  if(params.usePosition) {
    const Pose closest{getClosest(state)};
    if(abs(closest.v) > abs(pose.v)) {
      pose = closest;
      timedIndex = closestIndex;
      timer.setTime(path[timedIndex].t);
    }
  }
  return pose;
}

second_t Path::getTotalTime() {
  const second_t totalTime{path.back().t};
  return totalTime;
}

Path::Parameters Path::getParams() const {
  return params;
}

void Path::setDefaultParams(const Parameters &newParams) {
  defaultParams = newParams;
}

Pose Path::getClosest(const Pose &state) {
  meter_t previousDistance{distance(state, path[closestIndex])};
  int i;
  for(i = closestIndex; i < path.size(); i++) {
    const meter_t currentDistance{distance(state, path[i])};
    if(currentDistance > previousDistance) {
      break;
    }
    previousDistance = currentDistance;
  }
  closestIndex = i - 1;
  return path[closestIndex];
}

Pose Path::getTimed() {
  timer.start();
  int i;
  for(i = timedIndex; i < path.size(); i++) {
    if(timer.timeElapsed() < path[i].t) {
      break;
    }
  }
  timedIndex = i;
  if(timedIndex >= path.size()) {
    return path.back();
  }
  return path[timedIndex];
}

void Path::generate() {
  path.push_back(start);
  curvatures.push_back(getCurvature(0, getDerivative(0)));
  double t0{0.0};
  while(distance(path.back(), end) > params.spacing + params.maxSpacingError) {
    t0 = addNextPoint(t0);
  }
  path.push_back(end);
  curvatures.push_back(getCurvature(1, getDerivative(1)));
  parameterize();
}

void Path::parameterize() {
  beginParameterize();
  endParameterize();
  graphPath();
}

void Path::beginParameterize() {
  for(int i{1}; i < path.size() - 1; i++) {
    const meters_per_second_squared_t twoA{2.0 * params.maxA};
    const meters_per_second_t accelerated{
        sqrt(path[i - 1].v * path[i - 1].v + twoA * params.spacing)};
    path[i].v = units::math::min(accelerated, path[i].v);
    const int j{path.size() - 1 - i};
    const meters_per_second_t decelerated{
        sqrt(path[j + 1].v * path[j + 1].v + twoA * params.spacing)};
    path[j].v = units::math::min(decelerated, path[j].v);
  }
}

void Path::endParameterize() {
  path[0].t = 0_s;
  for(int i{1}; i < path.size(); i++) {
    path[i].omega = radians_per_second_t{
        getValueAs<meters_per_second_t>(path[i].v) * curvatures[i]};
    const meters_per_second_t avgV{(path[i - 1].v + path[i].v) / 2.0};
    path[i - 1].a = (path[i].v * path[i].v - path[i - 1].v * path[i - 1].v) /
                    (2.0 * params.spacing);
    path[i - 1].alpha = (path[i].omega * path[i].omega -
                         path[i - 1].omega * path[i - 1].omega) /
                        (2.0 * abs(difference(path[i].h, path[i - 1].h)));
    path[i].t = path[i - 1].t + params.spacing / avgV;
  }
  path.back().a = 0_mps_sq;
  path.back().alpha = 0_rad_per_s_sq;
}

double Path::addNextPoint(double t0) {
  double t2{1.0};
  double t1{t0 * params.binarySearchScaling +
            t2 * (1.0 - params.binarySearchScaling)};
  Pose p0{path.back()};
  Pose p1{getPoint(t1)};
  meter_t distanceToNext{distance(p0, p1)};
  while(abs(distanceToNext - params.spacing) > params.maxSpacingError) {
    if(distanceToNext > params.spacing) {
      t2 = t1;
    } else {
      t0 = t1;
    }
    t1 = t0 * params.binarySearchScaling +
         t2 * (1.0 - params.binarySearchScaling);
    p1 = getPoint(t1);
    distanceToNext = distance(p0, p1);
  }
  const Pose deriv{getDerivative(t1)};
  p1.h = getHeading(deriv);
  curvatures.push_back(getCurvature(t1, deriv));
  p1.v = units::math::min(
      params.maxV,
      params.maxV / std::abs(curvatures.back()) /
          getValueAs<meter_t>(params.track)); // Maybe multiply by 2?
  path.push_back(p1);
  return t1;
}

Pose Path::getPoint(const double t) const {
  const double t2{t * t};
  const double t3{t2 * t};
  const double threeT2{3.0 * t2};
  const double twoT3{2.0 * t3};
  return (twoT3 - threeT2 + 1.0) * start +
         (t3 - 2.0 * t2 + t) * startDirection + (-twoT3 + threeT2) * end +
         (t3 - t2) * endDirection;
}

degree_t Path::getHeading(const Pose &deriv) const {
  return 90_deg - atan2(deriv.y, deriv.x);
}

double Path::getCurvature(const double t, const UnwrappedPose &deriv) const {
  const UnwrappedPose deriv2{get2ndDerivative(t)};
  const double cross{deriv.x * deriv2.y - deriv.y * deriv2.x};
  if(!cross) {
    return infinitesimal;
  }
  const double denom{pow(deriv.x * deriv.x + deriv.y * deriv.y, 1.5)};
  return -cross / denom;
}

Pose Path::getDerivative(const double t) const {
  const double t2{t * t};
  const double threeT2{3 * t2};
  return (6.0 * t2 - 6.0 * t) * (start - end) +
         (threeT2 - 4.0 * t + 1.0) * startDirection +
         (threeT2 - 2.0 * t) * endDirection;
}

Pose Path::get2ndDerivative(const double t) const {
  const double sixT{6.0 * t};
  return (12.0 * t - 6.0) * (start - end) + (sixT - 4.0) * startDirection +
         (sixT - 2.0) * endDirection;
}

void Path::graphPath() {
  if(logger.getLevel() != Logger::Level::Debug) {
    return;
  }
  GUI::Map::clearSeries(GUI::SeriesColor::Red);
  // Can't handle all points on map, so only do those a bit apart from each
  // other.
  const int skip{8_in / params.spacing};
  for(int i{0}; i < path.size(); i++) {
    if(i % skip == 0) {
      GUI::Map::addPosition(path[i], GUI::SeriesColor::Red);
    }
  }
}

Path::Parameters Path::defaultParams{};
} // namespace atum