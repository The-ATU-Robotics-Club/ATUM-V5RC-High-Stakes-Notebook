#pragma once

#include <algorithm>
#include <utility>

namespace atum {
class SlewRate {
  public:
  explicit SlewRate(const std::pair<double, double> &rates);

  explicit SlewRate(const double rate);

  double slew(const double desired);

  double getOutput() const;

  private:
  double output{0};
  const double decRate;
  const double incRate;
};
} // namespace atum