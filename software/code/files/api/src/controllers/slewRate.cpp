#include "slewRate.hpp"

namespace atum {
SlewRate::SlewRate(const std::pair<double, double> &rates,
                   const double initialValue,
                   const Logger::Level loggerLevel) :
    decRate{std::abs(rates.first)},
    incRate{std::abs(rates.second)},
    output{initialValue},
    logger{loggerLevel} {
  logger.debug("Slew rate is constructed!");
}

SlewRate::SlewRate(const double rate,
                   const double initialValue,
                   const Logger::Level loggerLevel) :
    decRate{std::abs(rate)},
    incRate{std::abs(rate)},
    output{initialValue},
    logger{loggerLevel} {}

double SlewRate::slew(const double desired) {
  if(desired > output) {
    output = std::min(output + incRate, desired);
  } else {
    output = std::max(output - decRate, desired);
  }
  return getOutput(); // Use getOutput() for logging purposes.
}

double SlewRate::getOutput() {
  logger.debug("Output of slew rate is " + std::to_string(output));
  return output;
}
} // namespace atum