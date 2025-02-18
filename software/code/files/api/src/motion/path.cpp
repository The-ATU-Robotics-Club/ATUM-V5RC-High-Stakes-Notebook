#include "path.hpp"


namespace atum {
Path::Parameters::Parameters(const std::pair<meter_t, meter_t> &onAndOffRamps,
                             const meters_per_second_t iMaxV,
                             const meters_per_second_squared_t iMaxA,
                             const meters_per_second_squared_t iMaxD,
                             const meter_t iTrack,
                             const meter_t iSpacing,
                             const meter_t iMaxSpacingError,
                             const double iBinarySearchScaling) :
    onRamp{onAndOffRamps.first},
    offRamp{onAndOffRamps.second},
    maxV{iMaxV},
    maxA{iMaxA},
    maxD{iMaxD},
    track{iTrack},
    spacing{iSpacing},
    maxSpacingError{iMaxSpacingError},
    binarySearchScaling{iBinarySearchScaling} {}

Path::Parameters::Parameters(const meter_t ramp,
                             const meters_per_second_t iMaxV,
                             const meters_per_second_squared_t iMaxA,
                             const meters_per_second_squared_t iMaxD,
                             const meter_t iTrack,
                             const meter_t iSpacing,
                             const meter_t iMaxSpacingError,
                             const double iBinarySearchScaling) :
    onRamp{ramp},
    offRamp{ramp},
    maxV{iMaxV},
    maxA{iMaxA},
    maxD{iMaxD},
    track{iTrack},
    spacing{iSpacing},
    maxSpacingError{iMaxSpacingError},
    binarySearchScaling{iBinarySearchScaling} {}

Path::Parameters::Parameters(const Path::Parameters &other) :
    spacing{other.spacing},
    binarySearchScaling{other.binarySearchScaling} {
  onRamp = other.onRamp;
  offRamp = other.offRamp;
  maxV = other.maxV;
  maxA = other.maxA;
  maxD = other.maxD;
  track = other.track;
}

Path::Parameters &Path::Parameters::operator=(const Path::Parameters &other) {
  if(this == &other) {
    return *this;
  }
  if(other.onRamp) {
    onRamp = other.onRamp;
  }
  if(other.offRamp) {
    offRamp = other.offRamp;
  }
  if(other.maxV) {
    maxV = other.maxV;
  }
  if(other.maxA) {
    maxA = other.maxA;
  }
  if(other.maxD) {
    maxD = other.maxD;
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
      Pose{params.onRamp * cos(startH), params.onRamp * sin(startH)};
  const degree_t endH{90_deg - end.h};
  endDirection = Pose{params.offRamp * cos(endH), params.offRamp * sin(endH)};
  generate();
  logger.debug("Path has been generated!");
}

Pose Path::getPose(const int i) {
  return path[i];
}

int Path::getSize() const {
  return path.size();
}

Path::Parameters Path::getParams() const {
  return params;
}

void Path::setDefaultParams(const Parameters &newParams) {
  defaultParams = newParams;
}

void Path::generate() {
  path.push_back(start);
  double t0{0.0};
  while(distance(path.back(), end) > params.spacing + params.maxSpacingError) {
    t0 = addNextPoint(t0);
  }
  path.push_back(end);
  parameterize();
}

void Path::parameterize() {
  path[0].v = params.maxV;
  for(int i{path.size() - 2}; i >= 0; i--) {
    const meters_per_second_squared_t twoD{2.0 * params.maxD};
    const meters_per_second_t decelerated{
        sqrt(path[i + 1].v * path[i + 1].v + twoD * params.spacing)};
    path[i].v = units::math::min(decelerated, path[i].v);
  }
  graphPath();
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
  p1.v = units::math::min(
      params.maxV,
      params.maxV / std::abs(getCurvature(t1)) /
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

double Path::getCurvature(const double t) const {
  const UnwrappedPose deriv{getDerivative(t)};
  const UnwrappedPose deriv2{get2ndDerivative(t)};
  const double cross{deriv.x * deriv2.y - deriv.y * deriv2.x};
  if(!cross) {
    return infinitesimal;
  }
  const double denom{pow(deriv.x * deriv.x + deriv.y * deriv.y, 1.5)};
  return std::abs(cross / denom);
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

Path::Parameters Path::defaultParams{1_m};
} // namespace atum