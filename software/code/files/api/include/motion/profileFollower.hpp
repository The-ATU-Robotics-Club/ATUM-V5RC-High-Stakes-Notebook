#pragma once

#include "../controllers/controller.hpp"
#include "../time/timer.hpp"
#include "../utility/gui.hpp"
#include "motionProfile.hpp"

namespace atum {
template <typename X, typename V, typename A, typename J>
class ProfileFollower {
  public:
  using Profile = MotionProfile<X, V, A, J>;
  using MotionConstraints = Profile::MotionConstraints;
  using MotionStep = Profile::MotionStep;

  ProfileFollower(std::unique_ptr<Profile> iProfile,
                  std::shared_ptr<Controller> iPosition,
                  std::shared_ptr<Controller> iVelocity,
                  const double iKAccel,
                  const double iKDecel) :
      profile{std::move(iProfile)},
      position{iPosition},
      velocity{iVelocity},
      kAccel{iKAccel},
      kDecel{iKDecel} {}

  void reset(const X &target,
             const std::optional<MotionConstraints> constraints = {}) {
    profile->setParameters(target, constraints);
    position->reset();
    velocity->reset();
    timer.resetAlarm();
    reversed = {};
    beginDecel = false;
  }

  std::optional<double> getOutput(const X &currentX, const V &currentV) {
    const std::optional<MotionStep> step{
        profile->getMotionStep(timer.timeElapsed())};
    if(!step) return {};
    GUI::graph("Lateral Profile", std::make_pair(0, 60), 250);
    const double posOutput{position->getOutput(getValueAs<X>(currentX),
                                               getValueAs<X>(step.value().x))};
    GUI::redPlot(getValueAs<V>(currentV));
    GUI::greenPlot(getValueAs<V>(step.value().v));
    return getOutput(currentV, step.value()) + posOutput;
  }

  std::optional<double> getOutput(const V &currentV) {
    const std::optional<MotionStep> step{
        profile->getMotionStep(timer.timeElapsed())};
    if(!step) return {};
    return getOutput(currentV, step.value());
  }

  private:
  double getOutput(const V &currentV, const MotionStep &step) {
    double accOutput{getValueAs<A>(step.a)};
    if(!reversed.has_value() && accOutput) reversed = accOutput < 0.0;
    if((reversed.value_or(false) && accOutput >= 0.0) ||
       (!reversed.value_or(false) && accOutput < 0.0))
      accOutput *= kDecel;
    else
      accOutput *= kAccel;
    const double velOutput{
        velocity->getOutput(getValueAs<V>(currentV), getValueAs<V>(step.v))};
    return accOutput + velOutput;
  }

  std::unique_ptr<Profile> profile;
  std::shared_ptr<Controller> position;
  std::shared_ptr<Controller> velocity;
  const double kAccel;
  const double kDecel;
  Timer timer{};
  std::optional<bool> reversed{false};
  bool beginDecel{false};
};

using LateralProfileFollower = ProfileFollower<inch_t,
                                               inches_per_second_t,
                                               inches_per_second_squared_t,
                                               inches_per_second_cubed_t>;

using AngularProfileFollower = ProfileFollower<degree_t,
                                               degrees_per_second_t,
                                               degrees_per_second_squared_t,
                                               degrees_per_second_cubed_t>;
} // namespace atum