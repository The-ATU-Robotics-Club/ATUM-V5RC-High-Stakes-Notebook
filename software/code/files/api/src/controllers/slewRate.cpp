#include "slewRate.hpp"

namespace atum {
SlewRate::SlewRate(const double maxDecRate,
                   const double maxIncRate,
                   const Logger::Level loggerLevel) :
    decRate{std::abs(maxDecRate)},
    incRate{std::abs(maxIncRate)},
    logger{loggerLevel} {
  logger.debug("Slew rate is constructed!");
}

SlewRate::SlewRate(const double rate, const Logger::Level loggerLevel) :
    decRate{std::abs(rate)}, incRate{std::abs(rate)}, logger{loggerLevel} {}

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