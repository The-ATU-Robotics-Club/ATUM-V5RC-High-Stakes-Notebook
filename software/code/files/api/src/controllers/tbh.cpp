#include "tbh.hpp"

namespace atum {
TBH::TBH(const Parameters &iParams, const Logger::Level loggerLevel) :
    Controller{loggerLevel}, params{iParams} {
  logger.debug("TBH controller is constructed!");
}

double TBH::getOutput(const double error) {
  output += error * params.kTBH;
  output =
      std::clamp(output, params.constraints.first, params.constraints.second);
  if(std::signbit(error) != std::signbit(prevError)) {
    output = outputAtReference = 0.5 * (output + outputAtReference);
    prevError = error;
  }
  outputAtReference = output;
  output =
      std::clamp(output, params.constraints.first, params.constraints.second);
  return Controller::getOutput(); // Use getOutput() for logging purposes.
}

double TBH::getOutput(const double state, const double reference) {
  const double error{reference - state};
  if(reference != prevReference) {
    reset(reference, error);
  }
  return getOutput(error);
}

void TBH::reset() {
  outputAtReference = 0;
  prevReference = 0;
  prevError = 0;
}

TBH::Parameters TBH::getParams() const {
  return params;
}

void TBH::reset(const double reference, const double error) {
  prevReference = reference;
  prevError = error;
  output = outputAtReference = params.ff * reference;
}
} // namespace atum