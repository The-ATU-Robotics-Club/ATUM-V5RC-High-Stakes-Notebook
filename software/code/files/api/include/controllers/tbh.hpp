#pragma once

#include "controller.hpp"
#include <algorithm>
#include <cmath>

namespace atum {
class TBH : public Controller {
  public:
  struct Parameters {
    double kTBH{0};
    double ff{0};
    std::pair<double, double> constraints{
        -12000,
        12000};
  };
  
  explicit TBH(const Parameters &iParams);
  
  double getOutput(const double error) override;
  
  double getOutput(const double state, const double reference) override;

  void reset() override;
  
  Parameters getParams() const;

  private:
  void reset(const double reference, const double error);

  const Parameters params;
  double outputAtReference{0};
  double prevReference{0};
  double prevError{0};
};
} // namespace atum