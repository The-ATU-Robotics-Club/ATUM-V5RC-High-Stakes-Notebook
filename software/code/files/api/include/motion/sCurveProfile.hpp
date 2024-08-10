#pragma once

#include "motionProfile.hpp"

namespace atum {
template <typename X, typename V, typename A, typename J>
class SCurveProfile : public MotionProfile<X, V, A, J> {
  public:
  using MotionConstraints = MotionProfile<X, V, A, J>::MotionConstraints;
  using MotionStep = MotionProfile<X, V, A, J>::MotionStep;
  using MotionProfile<X, V, A, J>::defaultConstraints;

  // Use parent constructor.
  using MotionProfile<X, V, A, J>::MotionProfile;

  virtual void setParameters(const X &target,
                             const std::optional<MotionConstraints>
                                 &specialConstraints = {}) override {
    reset();
    MotionConstraints constraints = specialConstraints.value_or(defaultConstraints);
    const bool reachesMaxAccel{constraints.maxA * constraints.maxA /
                                   constraints.maxJ <
                               constraints.maxV};
    beginProfile(abs(target), constraints, reachesMaxAccel);
    finishProfile(target, constraints);
  }

  std::optional<MotionStep> getMotionStep(const second_t t) const override {
    MotionStep step;
    if(t < 0_s || t > totalTime) {
      return {};
    } else if(t < endJerking1Time) {
      step.x = jerk * t * t * t / 6.0;
      step.v = 0.5 * jerk * t * t;
      step.a = jerk * t;
    } else if(t < endConstantAccelTime) {
      const second_t dt{t - endJerking1Time};
      step.x = endJerking1Pos + startConstantAccelVel * dt +
               0.5 * startConstantAccelAccel * dt * dt;
      step.v = startConstantAccelVel + startConstantAccelAccel * dt;
      step.a = startConstantAccelAccel;
    } else if(t < endDejerking1Time) {
      const second_t dt{t - endConstantAccelTime};
      step.x = endConstantAccelPos + startDejerking1Vel * dt +
               0.5 * startDejerking1Accel * dt * dt +
               dejerk * dt * dt * dt / 6.0;
      step.v = startDejerking1Vel + startDejerking1Accel * dt +
               0.5 * dejerk * dt * dt;
      step.a = startDejerking1Accel + dejerk * dt;
    } else if(t < endConstantVelTime) {
      const second_t dt{t - endDejerking1Time};
      step.x = endDejerking1Pos + startConstantVelVel * dt;
      step.v = startConstantVelVel;
    } else if(t < endDejerking2Time) {
      const second_t dt{t - endConstantVelTime};
      step.x = endConstantVelPos + startDejerking2Vel * dt +
               dejerk * dt * dt * dt / 6.0;
      step.v = startDejerking2Vel + 0.5 * dejerk * dt * dt;
      step.a = dejerk * dt;
    } else if(t < endConstantDecelTime) {
      const second_t dt{t - endDejerking2Time};
      step.x = endDejerking2Pos + startConstantDecelVel * dt +
               0.5 * startConstantDecelAccel * dt * dt;
      step.v = startConstantDecelVel + startConstantDecelAccel * dt;
      step.a = startConstantDecelAccel;
    } else if(t < totalTime) {
      const second_t dt{t - endConstantDecelTime};
      step.x = endConstantDecelPos + startJerking2Vel * dt +
               0.5 * startJerking2Accel * dt * dt + jerk * dt * dt * dt / 6.0;
      step.v =
          startJerking2Vel + startJerking2Accel * dt + 0.5 * jerk * dt * dt;
      step.a = startJerking2Accel + jerk * dt;
    }

    return step;
  }

  private:
  void beginProfile(const X &target,
                    const MotionConstraints &constraints,
                    const bool reachesMaxAccel) {
    second_t jerkTime{constraints.maxA / constraints.maxJ};
    if(!reachesMaxAccel)
      jerkTime = second_t{sqrt(getValueAs<second_t>(jerkTime))};
    const X jerkDistance{constraints.maxJ * 2.0 * pow<3>(jerkTime)};
    X fullDistance{jerkDistance};
    if(reachesMaxAccel) {
      const second_t constantAccelTime =
          (constraints.maxV - constraints.maxJ * jerkTime * jerkTime) /
          constraints.maxA;
      fullDistance =
          2.0 * constraints.maxV * jerkTime +
          constraints.maxJ * jerkTime * jerkTime * constantAccelTime +
          constraints.maxA * constantAccelTime * constantAccelTime;
    }
    if(target < jerkDistance)
      profile4Stage(target, constraints);
    else if(!reachesMaxAccel)
      profile5Stage(target, constraints);
    else if(target < fullDistance)
      profile6Stage(target, constraints);
    else
      profileAllStages(target, constraints);
  }

  void profile4Stage(const X &target, const MotionConstraints &constraints) {
    endJerking1Time = endDejerking1Time = endDejerking2Time = totalTime =
        second_t{cbrt(getValueAs<X>(target) / getValueAs<J>(constraints.maxJ) /
                      2.0)};
    endJerking1Pos = endJerking1Time * endJerking1Time * endJerking1Time *
                     constraints.maxJ / 6.0;
    endDejerking1Pos = endDejerking2Pos = 0.5 * target - endJerking1Pos;
    startConstantAccelVel = startDejerking1Vel = startConstantDecelVel =
        startJerking2Vel =
            0.5 * constraints.maxJ * endJerking1Time * endJerking1Time;
    startConstantVelVel = startDejerking2Vel = startConstantAccelVel * 2.0;
    startConstantAccelAccel = startDejerking1Accel =
        constraints.maxJ * endJerking1Time;
    startConstantDecelAccel = startJerking2Accel = -startConstantAccelAccel;
  }

  void profile5Stage(const X &target, const MotionConstraints &constraints) {
    endJerking1Time = endDejerking1Time = endDejerking2Time = totalTime =
        sqrt(constraints.maxV / constraints.maxJ);
    endConstantVelTime =
        (target - constraints.maxV * endJerking1Time * 2.0) / constraints.maxV;
    endJerking1Pos = endJerking1Time * endJerking1Time * endJerking1Time *
                     constraints.maxJ / 6.0;
    endDejerking1Pos = endDejerking2Pos =
        constraints.maxV * endJerking1Time - endJerking1Pos;
    endConstantVelPos = target - endJerking1Pos * 2.0 - endDejerking1Pos * 2.0;
    startConstantAccelVel = startDejerking1Vel = startConstantDecelVel =
        startJerking2Vel = constraints.maxV / 2.0;
    startConstantVelVel = startDejerking2Vel = constraints.maxV;
    startConstantAccelAccel = startDejerking1Accel =
        constraints.maxJ * endJerking1Time;
    startConstantDecelAccel = startJerking2Accel = -startConstantAccelAccel;
  }

  void profile6Stage(const X &target, const MotionConstraints &constraints) {
    const A a{constraints.maxA};
    const V b{3.0 * constraints.maxA * constraints.maxA / constraints.maxJ};
    const X c{2.0 * constraints.maxA * constraints.maxA * constraints.maxA /
                  constraints.maxJ / constraints.maxJ -
              target};
    const auto discriminant = sqrt(b * b - 4.0 * a * c);
    const second_t t{
        max((-b + discriminant) / (2.0 * a), (-b - discriminant) / (2.0 * a))};
    endJerking1Time = endDejerking1Time = endDejerking2Time = totalTime =
        constraints.maxA / constraints.maxJ;
    endConstantAccelTime = endConstantDecelTime = t;
    startConstantAccelAccel = startDejerking1Accel =
        constraints.maxJ * endJerking1Time;
    startConstantDecelAccel = startJerking2Accel = -startConstantAccelAccel;
    startConstantAccelVel = startJerking2Vel =
        0.5 * constraints.maxJ * endJerking1Time * endJerking1Time;
    startDejerking1Vel = startConstantDecelVel =
        startConstantAccelVel + constraints.maxA * endConstantAccelTime;
    startConstantVelVel = startDejerking2Vel =
        startDejerking1Vel + startDejerking1Accel * endDejerking1Time -
        0.5 * constraints.maxJ * endDejerking1Time * endDejerking1Time;
    endJerking1Pos = constraints.maxJ * endJerking1Time * endJerking1Time *
                     endJerking1Time / 6.0;
    endConstantAccelPos = endConstantDecelPos =
        startConstantAccelVel * endConstantAccelTime +
        0.5 * startConstantAccelAccel * endConstantAccelTime *
            endConstantAccelTime;
    endDejerking1Pos = endDejerking2Pos =
        startDejerking1Vel * endDejerking1Time +
        0.5 * startDejerking1Accel * endDejerking1Time * endDejerking1Time -
        constraints.maxJ * endDejerking1Time * endDejerking1Time *
            endDejerking1Time / 6.0;
  }

  void profileAllStages(const X &target, const MotionConstraints &constraints) {
    endJerking1Time = constraints.maxA / constraints.maxJ;
    endJerking1Pos = constraints.maxJ * endJerking1Time * endJerking1Time *
                     endJerking1Time / 6.0;
    startConstantAccelVel =
        0.5 * constraints.maxJ * endJerking1Time * endJerking1Time;
    startConstantAccelAccel = constraints.maxA;
    endConstantAccelTime =
        (constraints.maxV - startConstantAccelVel * 2.0) / constraints.maxA;
    endConstantAccelPos = startConstantAccelVel * endConstantAccelTime +
                          0.5 * startConstantAccelAccel * endConstantAccelTime *
                              endConstantAccelTime;
    startDejerking1Vel =
        startConstantAccelVel + startConstantAccelAccel * endConstantAccelTime;
    startDejerking1Accel = constraints.maxA;
    endDejerking1Time = endJerking1Time;
    endDejerking1Pos =
        startDejerking1Vel * endDejerking1Time +
        0.5 * startDejerking1Accel * endDejerking1Time * endDejerking1Time -
        constraints.maxJ * endDejerking1Time * endDejerking1Time *
            endDejerking1Time / 6.0;
    startConstantVelVel = constraints.maxV;
    endConstantVelTime = (target - 2.0 * (endJerking1Pos + endConstantAccelPos +
                                          endDejerking1Pos)) /
                         constraints.maxV;
    endConstantVelPos = startConstantVelVel * endConstantVelTime;
    startDejerking2Vel = constraints.maxV;
    endDejerking2Time = endDejerking1Time;
    endDejerking2Pos = endDejerking1Pos;
    startConstantDecelVel = startDejerking1Vel;
    startConstantDecelAccel = -constraints.maxA;
    endConstantDecelTime = endConstantAccelTime;
    endConstantDecelPos = endConstantAccelPos;
    startJerking2Vel = startConstantAccelVel;
    startJerking2Accel = -constraints.maxA;
    totalTime = endJerking1Time;
  }

  void finishProfile(const X &target, const MotionConstraints &constraints) {
    jerk = constraints.maxJ;
    dejerk = -constraints.maxJ;
    if(target < X{0.0}) reverse();
    integrate();
  }

  void reverse() {
    reversePos();
    reverseVel();
    reverseAccel();
    reverseJerk();
  }

  void reversePos() {
    endJerking1Pos *= -1;
    endConstantAccelPos *= -1;
    endDejerking1Pos *= -1;
    endConstantVelPos *= -1;
    endDejerking2Pos *= -1;
    endConstantDecelPos *= -1;
  }

  void reverseVel() {
    startConstantAccelVel *= -1;
    startDejerking1Vel *= -1;
    startConstantVelVel *= -1;
    startDejerking2Vel *= -1;
    startConstantDecelVel *= -1;
    startJerking2Vel *= -1;
  }

  void reverseAccel() {
    startConstantAccelAccel *= -1;
    startDejerking1Accel *= -1;
    startConstantDecelAccel *= -1;
    startJerking2Accel *= -1;
  }

  void reverseJerk() {
    jerk *= -1;
    dejerk *= -1;
  }

  void integrate() {
    integrateTime();
    integratePos();
  }

  void integrateTime() {
    endConstantAccelTime += endJerking1Time;
    endDejerking1Time += endConstantAccelTime;
    endConstantVelTime += endDejerking1Time;
    endDejerking2Time += endConstantVelTime;
    endConstantDecelTime += endDejerking2Time;
    totalTime += endConstantDecelTime;
  }

  void integratePos() {
    endConstantAccelPos += endJerking1Pos;
    endDejerking1Pos += endConstantAccelPos;
    endConstantVelPos += endDejerking1Pos;
    endDejerking2Pos += endConstantVelPos;
    endConstantDecelPos += endDejerking2Pos;
  }

  void reset() {
    endJerking1Time = 0_s;
    endConstantAccelTime = 0_s;
    endDejerking1Time = 0_s;
    endConstantVelTime = 0_s;
    endDejerking2Time = 0_s;
    endConstantDecelTime = 0_s;
    totalTime = 0_s;
    endJerking1Pos = X{0.0};
    endConstantAccelPos = X{0.0};
    endDejerking1Pos = X{0.0};
    endConstantVelPos = X{0.0};
    endDejerking2Pos = X{0.0};
    endConstantDecelPos = X{0.0};
    startConstantAccelVel = V{0.0};
    startDejerking1Vel = V{0.0};
    startConstantVelVel = V{0.0};
    startDejerking2Vel = V{0.0};
    startConstantDecelVel = V{0.0};
    startJerking2Vel = V{0.0};
    startConstantAccelAccel = A{0.0};
    startDejerking1Accel = A{0.0};
    startConstantDecelAccel = A{0.0};
    startJerking2Accel = A{0.0};
    jerk = J{0.0};
    dejerk = J{0.0};
  }

  second_t endJerking1Time, endConstantAccelTime, endDejerking1Time,
      endConstantVelTime, endDejerking2Time, endConstantDecelTime, totalTime;
  X endJerking1Pos, endConstantAccelPos, endDejerking1Pos, endConstantVelPos,
      endDejerking2Pos, endConstantDecelPos;
  V startConstantAccelVel, startDejerking1Vel, startConstantVelVel,
      startDejerking2Vel, startConstantDecelVel, startJerking2Vel;
  A startConstantAccelAccel, startDejerking1Accel, startConstantDecelAccel,
      startJerking2Accel;
  J jerk, dejerk;
};

using LateralSCurveProfile = SCurveProfile<inch_t,
                                           inches_per_second_t,
                                           inches_per_second_squared_t,
                                           inches_per_second_cubed_t>;

using AngularSCurveProfile = SCurveProfile<degree_t,
                                           degrees_per_second_t,
                                           degrees_per_second_squared_t,
                                           degrees_per_second_cubed_t>;
} // namespace atum