#pragma once

#include "motionProfile.hpp"

namespace atum {
template <typename X, typename V, typename A, typename J>
class TrapezoidalProfile : public MotionProfile<X, V, A, J> {
  public:
  using MotionConstraints = MotionProfile<X, V, A, J>::MotionConstraints;
  using MotionStep = MotionProfile<X, V, A, J>::MotionStep;
  using MotionProfile<X, V, A, J>::defaultConstraints;

  // Use parent constructor.
  using MotionProfile<X, V, A, J>::MotionProfile;

  virtual void setParameters(const X &target,
                             const std::optional<MotionConstraints>
                                 &specialConstraints = {}) override {
    MotionConstraints constraints = defaultConstraints;
    if(specialConstraints) {
      if(specialConstraints.value().maxV != V{0.0})
        constraints.maxV = specialConstraints.value().maxV;
      if(specialConstraints.value().maxA != A{0.0})
        constraints.maxA = specialConstraints.value().maxA;
      if(specialConstraints.value().maxJ != J{0.0})
        constraints.maxJ = specialConstraints.value().maxJ;
    }
    const X distanceWhileAccel{constraints.maxV * constraints.maxV /
                               constraints.maxA};
    if(target > distanceWhileAccel)
      profileMaxV(abs(target), constraints);
    else
      profileNoMaxV(abs(target), constraints);
    finishProfile(target, constraints);
  }

  std::optional<MotionStep> getMotionStep(const second_t t) const override {
    MotionStep step;
    if(t < 0_s || t > totalTime) {
      return {};
    } else if(t < endAccelTime) {
      step.x = 0.5 * accel * t * t;
      step.v = accel * t;
      step.a = accel;
    } else if(t < endConsantVelTime) {
      const second_t dt{t - endAccelTime};
      step.x = endAccelPos + startConstantVelVel * dt;
      step.v = startConstantVelVel;
      step.a = A{0.0};
    } else if(t < totalTime) {
      const second_t dt{t - endConsantVelTime};
      step.x = endConstantVelPos + startDecelVel * dt + 0.5 * decel * dt * dt;
      step.v = startDecelVel + decel * dt;
      step.a = decel;
    }
    return step;
  }

  private:
  void profileMaxV(const X &target, const MotionConstraints &constraints) {
    endAccelTime = constraints.maxV / constraints.maxA;
    endAccelPos = 0.5 * constraints.maxV * constraints.maxV / constraints.maxA;
    endConsantVelTime = (target - 2.0 * endAccelPos) / constraints.maxV;
    endConstantVelPos = 0.5 * constraints.maxV * endConsantVelTime;
    startConstantVelVel = constraints.maxV;
    totalTime = endAccelTime;
    startDecelVel = startConstantVelVel;
  }

  void profileNoMaxV(const X &target, const MotionConstraints &constraints) {
    endAccelTime = sqrt(target / constraints.maxA);
    endAccelPos = target / 2.0;
    endConsantVelTime = 0_s;
    endConstantVelPos = X{0.0};
    startConstantVelVel = constraints.maxA * endAccelTime;
    totalTime = endAccelTime;
    startDecelVel = startConstantVelVel;
  }

  void finishProfile(const X &target, const MotionConstraints &constraints) {
    accel = constraints.maxA;
    decel = -constraints.maxA;
    if(target < X{0.0}) reverse();
    integrate();
  }

  void reverse() {
    endAccelPos *= -1;
    endConstantVelPos *= -1;
    startConstantVelVel *= -1;
    startDecelVel *= -1;
    accel *= -1;
    decel *= -1;
  }

  void integrate() {
    endConsantVelTime += endAccelTime;
    totalTime += endConsantVelTime;
    endConstantVelPos += endAccelPos;
  }

  second_t endAccelTime, endConsantVelTime, totalTime;
  X endAccelPos, endConstantVelPos;
  V startConstantVelVel, startDecelVel;
  A accel, decel;
};

using LateralTrapezoidalProfile = TrapezoidalProfile<foot_t,
                                                     feet_per_second_t,
                                                     feet_per_second_squared_t,
                                                     feet_per_second_cubed_t>;

using AngularTrapezoidalProfile =
    TrapezoidalProfile<degree_t,
                       degrees_per_second_t,
                       degrees_per_second_squared_t,
                       degrees_per_second_cubed_t>;
} // namespace atum