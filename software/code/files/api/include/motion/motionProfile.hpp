// Credit to 4253B for this method of achieving motion profiling.

#pragma once

#include "../utility/units.hpp"

namespace atum {
template <typename X, typename V, typename A, typename J>
class MotionProfile {
  public:
  struct MotionConstraints {
    V maxV;
    A maxA{0.0};
    J maxJ{0.0}; // Ignored for certain profiles.
  };

  struct MotionStep {
    X x;
    V v;
    A a;
  };

  MotionProfile(const MotionConstraints &iDefaultConstraints) :
      defaultConstraints{iDefaultConstraints} {}

  virtual void setParameters(
      const X &target,
      const std::optional<MotionConstraints> &specialConstraints = {}) = 0;

  virtual std::optional<MotionStep> getMotionStep(const second_t t) const = 0;

  protected:
  const MotionConstraints defaultConstraints;
};

using LateralMotionProfile = MotionProfile<inch_t,
                                           inches_per_second_t,
                                           inches_per_second_squared_t,
                                           inches_per_second_cubed_t>;

using AngularMotionProfile = MotionProfile<degree_t,
                                           degrees_per_second_t,
                                           degrees_per_second_squared_t,
                                           degrees_per_second_cubed_t>;
} // namespace atum