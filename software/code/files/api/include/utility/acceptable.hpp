#pragma once

#include "../time/timer.hpp"
#include "units.hpp"
#include <limits>

namespace atum {
template <typename U = double, typename dU = double>
class Acceptable {
  public:
  Acceptable(const second_t iTimeout,
             const U &iMaxError = U{0.0},
             const dU &iMaxDeriv = dU{std::numeric_limits<double>::max()},
             const second_t minTime = 0_s) :
      timeout{iTimeout},
      maxError{iMaxError},
      maxDeriv{iMaxDeriv},
      minTimer{minTime} {}

  bool canAccept(const U &state, const U &reference) {
    return canAccept(reference - state);
  }

  bool canAccept(const U &error) {
    if(!timeoutTimer) timeoutTimer = Timer{timeout};
    const second_t currentTime{time()};
    const dU deriv{(error - prevError) / (currentTime - prevTime)};
    accepted = abs(error) <= maxError;
    accepted = accepted && abs(deriv) <= maxDeriv;
    if(!accepted) minTimer.resetAlarm();
    accepted = accepted && minTimer.goneOff();
    // If given timeout has been passed, accepted should always be true.
    accepted = accepted || timeoutTimer.value().goneOff();
    prevError = error;
    prevTime = currentTime;
    return accepted;
  }

  bool canAccept() {
    return accepted;
  }

  private:
  bool accepted{false};
  const second_t timeout;
  const U maxError;
  const dU maxDeriv;
  Timer minTimer;
  std::optional<Timer> timeoutTimer;
  U prevError{0};
  second_t prevTime{0_s};
};

using AcceptableDistance = Acceptable<inch_t, feet_per_second_t>;

using AcceptableAngle = Acceptable<degree_t, degrees_per_second_t>;
} // namespace atum