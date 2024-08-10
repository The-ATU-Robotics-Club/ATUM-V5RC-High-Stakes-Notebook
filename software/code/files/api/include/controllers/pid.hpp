#pragma once

#include "controller.hpp"
#include <algorithm>
#include <cmath>

namespace atum {
class PID : public Controller {
  public:
  struct Parameters {
    const double kP{0};
    const double kI{0};
    const double kD{0};
    const double ff{0};
    const std::pair<double, double> constraints{-12000, 12000};
    const double threshI{std::numeric_limits<long double>::max()};
  };

  PID(const Parameters &iParams);

  double getOutput(const double error) override;

  double getOutput(const double state, const double reference) override;

  void reset() override;

  Parameters getParams() const;

  private:
  const Parameters params;
  double I{0};
  double prevState{0};
  double prevError{0};
};
}; // namespace atum