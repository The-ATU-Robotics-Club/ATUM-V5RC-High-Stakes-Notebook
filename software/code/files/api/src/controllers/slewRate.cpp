#include "slewRate.hpp"

namespace atum {
SlewRate::SlewRate(const std::pair<double, double> &rates) :
    decRate{std::abs(rates.first)}, incRate{std::abs(rates.second)} {}

SlewRate::SlewRate(const double rate) :
    decRate{std::abs(rate)}, incRate{std::abs(rate)} {}

double SlewRate::slew(const double desired) {
  if(desired > output)
    output = std::min(output + incRate, desired);
  else
    output = std::max(output - decRate, desired);
  return output;
}

double SlewRate::getOutput() const {
  return output;
}
} // namespace atum