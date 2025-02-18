/**
 * @file motionProfile.hpp
 * @brief Includes the MotionProfile template class and some helpful aliases.
 * @date 2024-12-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../time/timer.hpp"
#include "../utility/logger.hpp"
#include "kinematics.hpp"
#include <limits>


namespace atum {
/**
 * @brief This class supports the efficient creation of S and trapezoidal motion
 * profiles, alongside helpful logging and graphing functionalities.
 *
 * Can be polled by position to allow for robust following. This works by
 * comparing the closest point in time and closest point in position along the
 * profile and using the point with the larger velocity.
 *
 * @tparam Unit
 */
template <typename Unit>
class MotionProfile {
  public:
  // Figure out the corresponding types for the derivative of Unit.
  using UnitsPerSecond = decltype(Unit{1} / 1_s);
  using UnitsPerSecondSq = decltype(UnitsPerSecond{1} / 1_s);
  using UnitsPerSecondCb = decltype(UnitsPerSecondSq{1} / 1_s);
  using UnitKinematics = Kinematics<Unit>;

  /**
   * @brief The parameters involved in creating the motion profile.
   *
   */
  struct Parameters {
    /**
     * @brief Constructs a new Parameters object. Defaults are zero to prevent
     * unwanted overwriting when assigning.
     *
     * @param iMaxV
     * @param iMaxA
     * @param iMaxJ
     * @param iUsePosition
     * @param iSearchIterations
     */
    Parameters(const UnitsPerSecond iMaxV = UnitsPerSecond{0.0},
               const UnitsPerSecondSq iMaxA = UnitsPerSecondSq{0.0},
               const UnitsPerSecondCb iMaxJ = UnitsPerSecondCb{0.0},
               const bool iUsePosition = true,
               const std::size_t iSearchIterations = 25) :
        maxV{iMaxV},
        maxA{iMaxA},
        maxJ{iMaxJ},
        usePosition{iUsePosition},
        searchIterations{iSearchIterations} {}

    /**
     * @brief Constructs a new Parameters object with another Parameters object.
     *
     * @param other
     */
    Parameters(const Parameters &other) {
      maxV = other.maxV;
      maxA = other.maxA;
      maxJ = other.maxJ;
      usePosition = other.usePosition;
      searchIterations = other.searchIterations;
    }

    /**
     * @brief Used for reseting parameters to default values.
     *
     * @param other
     */
    void reset(const Parameters &other) {
      maxV = other.maxV;
      maxA = other.maxA;
      maxJ = other.maxJ;
      usePosition = other.usePosition;
      searchIterations = other.searchIterations;
    }

    /**
     * @brief Used for reseting parameters to special values.
     *
     * @param other
     */
    void setSpecial(const Parameters &other) {
      if(other.maxV) {
        maxV = other.maxV;
      }
      if(other.maxA) {
        maxA = other.maxA;
      }
      if(other.maxJ) {
        maxJ = other.maxJ;
      }
    }

    UnitsPerSecond maxV{0.0};
    UnitsPerSecondSq maxA{0.0};
    // Default to a trapezoidal profile.
    UnitsPerSecondCb maxJ{0.0};
    bool usePosition{true};
    // The number of iterations in the binary search for the closest point on
    // the motion profile.
    std::size_t searchIterations{25};
  };

  /**
   * @brief The points along a motion profile with relevant information to
   * follow.
   *
   */
  struct Point {
    Unit s{0.0};
    UnitsPerSecond v{0.0};
    UnitsPerSecondSq a{0.0};
    UnitsPerSecondCb j{0.0};
    second_t t{0.0};
  };

  /**
   * @brief Constructs a motion profile with the given parameters. Need to call
   * generate before getting points.
   *
   * @param iDefaultParams
   * @param loggerLevel
   */
  MotionProfile(const Parameters &iDefaultParams,
                const Logger::Level &loggerLevel = Logger::Level::Info) :
      defaultParams{iDefaultParams},
      params{iDefaultParams},
      logger{loggerLevel} {
    logger.debug("Motion profile has been constructed!");
  }

  /**
   * @brief Generates the relevant points of the motion profile based on the
   * given start and end position, as well as any special parameters (0 values
   * will use the default parameters).
   *
   * @param iStart
   * @param iEnd
   * @param specialParameters
   */
  void generate(const Unit iStart,
                const Unit iEnd,
                const Parameters &specialParameters = {}) {
    // Set params to defaults to reset from potential
    // previous special parameters.
    params.reset(defaultParams);
    start = iStart;
    end = iEnd;
    target = end - start;
    for(Point &p : points) {
      p = Point{};
    }
    params.setSpecial(specialParameters);
    prepareGraphing();
    beginProfile();
    finishProfile();
    timer.restart();
    logger.debug("Motion profile going from " + to_string(start) + " to " +
                 to_string(end) + " has been generated!");
  }

  /**
   * @brief Gets a point along the motion profile based on time along the
   * profile and the given position (whichever gives a higher velocity).
   *
   * The default of zero for the position parameter allows for easy use in
   * time-only polling contexts.
   *
   * @param s
   * @return Point
   */
  Point getPoint(const Unit s = Unit{0}) {
    Point point{getTimedPoint()};
    if(params.usePosition) {
      const Point closestPoint{getClosestPoint(s)};
      if(abs(closestPoint.v) > abs(point.v)) {
        timer.setTime(closestPoint.t);
        point = closestPoint;
      }
    }
    graphPoint(point);
    return point;
  }

  /**
   * @brief Gets the total time the profile should take to complete.
   *
   * @return second_t
   */
  second_t getTotalTime() const {
    return points[6].t;
  }

  /**
   * @brief Gets the parameters for the profile.
   *
   * @return Parameters
   */
  Parameters getParameters() const {
    return params;
  }

  /**
   * @brief Returns if the profile involves moving backwards.
   *
   * @return true
   * @return false
   */
  bool isBackwards() const {
    return target < Unit{0};
  }

  private:
  /**
   * @brief Gets a point based on the internal timer.
   *
   * @return Point
   */
  Point getTimedPoint() {
    timer.start();
    const second_t t{timer.timeElapsed()};
    return getPointAt(t);
  }

  /**
   * @brief Gets the closest point to the given position.
   *
   * @param s
   * @return Point
   */
  Point getClosestPoint(const Unit s) {
    // Return appropriate value if behind start or in front of end.
    if(distance(s, start) > Unit{0}) {
      return {start,
              UnitsPerSecond{0},
              UnitsPerSecondSq{0},
              UnitsPerSecondCb{(isBackwards()) ? -params.maxJ : params.maxJ},
              0_s};
    } else if(distance(s, end) < Unit{0}) {
      return {end,
              UnitsPerSecond{0},
              UnitsPerSecondSq{0},
              UnitsPerSecondCb{0},
              points[6].t};
    }
    const auto [t0, t2] = getTimeBounds(s);
    const second_t t1{searchForClosestPointTime(s, t0, t2)};
    return getPointAt(t1);
  }

  /**
   * @brief Calculates the reference point at a given time along the profile.
   *
   * @param t
   * @return Point
   */
  Point getPointAt(const second_t t) {
    if(t < 0_s) {
      return {start,
              UnitsPerSecond{0},
              UnitsPerSecondSq{0},
              UnitsPerSecondCb{(isBackwards()) ? -params.maxJ : params.maxJ},
              0_s};
    } else if(t >= points[6].t) {
      return {end,
              UnitsPerSecond{0},
              UnitsPerSecondSq{0},
              UnitsPerSecondCb{0},
              points[6].t};
    }
    int i{0};
    while(t >= points[i].t) {
      i++;
    }
    second_t dt{t};
    if(i > 0) {
      dt -= points[i - 1].t;
    }
    Point p;
    p.s = UnitKinematics::position(
        dt, points[i].s, points[i].v, points[i].a, points[i].j);
    p.v = UnitKinematics::velocity(dt, points[i].v, points[i].a, points[i].j);
    p.a = points[i].a + points[i].j * dt;
    p.j = points[i].j;
    p.t = t;
    return p;
  }

  /**
   * @brief Gets the time interval that the given position lies in between.
   *
   * @param s
   * @return std::pair<second_t, second_t>
   */
  std::pair<second_t, second_t> getTimeBounds(const Unit s) {
    // Finds first point we're past (starts with 1 since we know we're past the
    // start if this ever gets called).
    int i{1};
    while(i < 7 && distance(s, points[i].s) < Unit{0}) {
      i++;
    }
    if(i < 2) {
      return {0_s, points[0].t};
    } else if(i >= 7) {
      return {points[5].t, points[6].t};
    } else {
      return {points[i - 2].t, points[i - 1].t};
    }
  }

  /**
   * @brief Performs a binary search starting with the two given times to find
   * the closest point to the given position.
   *
   * @param s
   * @param t0
   * @param t2
   * @return second_t
   */
  second_t searchForClosestPointTime(const Unit s, second_t t0, second_t t2) {
    for(std::size_t n{0}; n < params.searchIterations; n++) {
      const second_t t1{(t0 + t2) / 2.0};
      if(distance(s, getPointAt(t1).s) >= Unit{0}) {
        t2 = t1;
      } else {
        t0 = t1;
      }
    }
    return (t0 + t2) / 2.0;
  }

  /**
   * @brief Gets the difference between the right and left sides parameters and
   * reverses as necessary.
   *
   * @param left
   * @param right
   * @return Unit
   */
  Unit distance(const Unit left, const Unit right) {
    Unit diff{right - left};
    if(isBackwards()) {
      diff *= -1;
    }
    return diff;
  }

  /**
   * @brief Begins the profiling process by determining how many stages the
   * profile has.
   *
   */
  void beginProfile() {
    const bool reachesMaxAccel{params.maxA * params.maxA / params.maxJ <
                               params.maxV};
    second_t jerkT{params.maxA / params.maxJ};
    if(!reachesMaxAccel) {
      jerkT = second_t{sqrt(getValueAs<second_t>(jerkT))};
    }
    const Unit jerkDistance{params.maxJ * 2.0 * pow<3>(jerkT)};
    Unit fullDistance{jerkDistance};
    if(reachesMaxAccel) {
      const second_t constantAccelT =
          (params.maxV - params.maxJ * jerkT * jerkT) / params.maxA;
      fullDistance = 2.0 * params.maxV * jerkT +
                     params.maxJ * jerkT * jerkT * constantAccelT +
                     params.maxA * constantAccelT * constantAccelT;
    }
    const Unit absTarget{abs(target)};
    if(absTarget < jerkDistance) {
      profile4Stage();
    } else if(!reachesMaxAccel) {
      profile5Stage();
    } else if(absTarget < fullDistance) {
      profile6Stage();
    } else {
      profileAllStages();
    }
  }

  /**
   * @brief Generate a profile with no zero jerk section.
   *
   */
  void profile4Stage() {
    logger.debug("Generated profile is 4 stages.");
    const Unit absTarget{abs(target)};
    points[0].t = points[2].t = points[4].t = points[6].t =
        second_t{cbrt(getValueAs<Unit>(absTarget) /
                      getValueAs<UnitsPerSecondCb>(params.maxJ) / 2.0)};
    points[1].s = points[0].t * points[0].t * points[0].t * params.maxJ / 6.0;
    points[3].s = points[5].s = 0.5 * absTarget - points[1].s;
    points[1].v = points[2].v = points[5].v = points[6].v =
        0.5 * params.maxJ * points[0].t * points[0].t;
    points[3].v = points[4].v = points[1].v * 2.0;
    points[1].a = points[2].a = params.maxJ * points[0].t;
    points[5].a = points[6].a = -points[1].a;
  }

  /**
   * @brief Generate a profile that never reaches max acceleration.
   *
   */
  void profile5Stage() {
    logger.debug("Generated profile is 5 stages.");
    const Unit absTarget{abs(target)};
    points[0].t = points[2].t = points[4].t = points[6].t =
        sqrt(params.maxV / params.maxJ);
    points[3].t = (absTarget - params.maxV * points[0].t * 2.0) / params.maxV;
    points[1].s = points[0].t * points[0].t * points[0].t * params.maxJ / 6.0;
    points[3].s = points[5].s = params.maxV * points[0].t - points[1].s;
    points[4].s = absTarget - points[1].s * 2.0 - points[3].s * 2.0;
    points[1].v = points[2].v = points[5].v = points[6].v = params.maxV / 2.0;
    points[3].v = points[4].v = params.maxV;
    points[1].a = points[2].a = params.maxJ * points[0].t;
    points[5].a = points[6].a = -points[1].a;
  }

  /**
   * @brief Generate a profile that never reaches max velocity.
   *
   */
  void profile6Stage() {
    logger.debug("Generated profile is 6 stages.");
    const Unit absTarget{abs(target)};
    const UnitsPerSecond b{3.0 * params.maxA * params.maxA / params.maxJ};
    const Unit c{2.0 * params.maxA * params.maxA * params.maxA / params.maxJ /
                     params.maxJ -
                 absTarget};
    const auto discriminant = sqrt(b * b - 4.0 * params.maxA * c);
    const second_t t{max((-b + discriminant) / (2.0 * params.maxA),
                         (-b - discriminant) / (2.0 * params.maxA))};
    points[0].t = points[2].t = points[4].t = points[6].t =
        params.maxA / params.maxJ;
    points[1].t = points[5].t = t;
    points[1].a = points[2].a = params.maxJ * points[0].t;
    points[5].a = points[6].a = -points[1].a;
    points[1].v = points[6].v = 0.5 * params.maxJ * points[0].t * points[0].t;
    points[2].v = points[5].v = points[1].v + params.maxA * points[1].t;
    points[3].v = points[4].v = points[2].v + points[2].a * points[2].t -
                                0.5 * params.maxJ * points[2].t * points[2].t;
    points[1].s = params.maxJ * points[0].t * points[0].t * points[0].t / 6.0;
    points[2].s = points[6].s = points[1].v * points[1].t +
                                0.5 * points[1].a * points[1].t * points[1].t;
    points[3].s = points[5].s =
        points[2].v * points[2].t +
        0.5 * points[2].a * points[2].t * points[2].t -
        params.maxJ * points[2].t * points[2].t * points[2].t / 6.0;
  }

  /**
   * @brief Generate a profile that reaches max velocity.
   *
   */
  void profileAllStages() {
    logger.debug("Generated profile is 7 (all) stages.");
    const Unit absTarget{abs(target)};
    points[0].t = points[2].t = points[4].t = points[6].t =
        params.maxA / params.maxJ;
    points[1].s = params.maxJ * points[0].t * points[0].t * points[0].t / 6.0;
    points[1].v = points[6].v = 0.5 * params.maxJ * points[0].t * points[0].t;
    points[1].a = points[2].a = params.maxA;
    points[1].t = points[5].t = (params.maxV - points[1].v * 2.0) / params.maxA;
    points[2].s = points[6].s = points[1].v * points[1].t +
                                0.5 * points[1].a * points[1].t * points[1].t;
    points[2].v = points[5].v = points[1].v + points[1].a * points[1].t;
    points[3].s = points[5].s =
        points[2].v * points[2].t +
        0.5 * points[2].a * points[2].t * points[2].t -
        params.maxJ * points[2].t * points[2].t * points[2].t / 6.0;
    points[3].v = points[4].v = params.maxV;
    points[3].t =
        (absTarget - 2.0 * (points[1].s + points[2].s + points[3].s)) /
        params.maxV;
    points[4].s = points[3].v * points[3].t;
    points[5].a = points[6].a = -params.maxA;
  }

  /**
   * @brief Finish up the profile by setting the jerk at each point, reversing
   * if necessary, and integrating time and position.
   *
   */
  void finishProfile() {
    points[0].j = points[6].j = params.maxJ;
    points[2].j = points[4].j = -params.maxJ;
    if(target < Unit{0.0}) {
      for(Point &p : points) {
        p.s *= -1;
        p.v *= -1;
        p.a *= -1;
        p.j *= -1;
      }
    }
    points[0].s = start;
    for(std::size_t i{1}; i < 7; i++) {
      points[i].t += points[i - 1].t;
      points[i].s += points[i - 1].s;
    }
  }

  /**
   * @brief Prepare the graph screen.
   *
   */
  void prepareGraphing() {
    if(logger.getLevel() != Logger::Level::Debug) {
      return;
    }
    GUI::Graph::clearSeries(GUI::SeriesColor::White);
    GUI::Graph::clearSeries(GUI::SeriesColor::Magenta);
    GUI::Graph::clearSeries(GUI::SeriesColor::Red);
    GUI::Graph::clearSeries(GUI::SeriesColor::Blue);
    const double rawStart{getValueAs<Unit>(start)};
    const double rawEnd{getValueAs<Unit>(end)};
    GUI::Graph::setSeriesRange(
        {std::min(rawStart, rawEnd), std::max(rawStart, rawEnd)},
        GUI::SeriesColor::White);
    GUI::Graph::setSeriesRange(getValueAs<UnitsPerSecond>(params.maxV),
                               GUI::SeriesColor::Magenta);
    GUI::Graph::setSeriesRange(getValueAs<UnitsPerSecondSq>(params.maxA),
                               GUI::SeriesColor::Red);
    GUI::Graph::setSeriesRange(getValueAs<UnitsPerSecondCb>(params.maxJ),
                               GUI::SeriesColor::Blue);
  }

  /**
   * @brief Graphs a point.
   *
   * @param p
   */
  void graphPoint(const Point &p) {
    if(logger.getLevel() != Logger::Level::Debug) {
      return;
    }
    GUI::Graph::addValue(getValueAs<Unit>(p.s), GUI::SeriesColor::White);
    GUI::Graph::addValue(getValueAs<UnitsPerSecond>(p.v),
                         GUI::SeriesColor::Magenta);
    GUI::Graph::addValue(getValueAs<UnitsPerSecondSq>(p.a),
                         GUI::SeriesColor::Red);
    GUI::Graph::addValue(getValueAs<UnitsPerSecondCb>(p.j),
                         GUI::SeriesColor::Blue);
  }

  Unit start;
  Unit end;
  Unit target;
  const Parameters defaultParams;
  Parameters params;
  std::array<Point, 7> points;
  Logger logger;
  Timer timer;
};

/**
 * @brief Specifically produce motion profiles for lateral and angular
 * contexts and make them easy to access.
 *
 */
using LateralProfile = MotionProfile<meter_t>;
using AngularProfile = MotionProfile<radian_t>;
} // namespace atum