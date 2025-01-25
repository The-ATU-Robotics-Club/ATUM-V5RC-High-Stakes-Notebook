#include "pid.hpp"

namespace atum {
PID::PID(const Parameters &iParams, const Logger::Level loggerLevel) :
    Controller{loggerLevel}, params{iParams} {
  logger.debug("PID controller is constructed!");
};

double PID::getOutput(const double error) {
  const double P{params.kP * error};
  const double D{params.kD * (error - prevError)};
  updateI(error); // Updates prevError as side effect, so must be after D.
  if(params.ffScaling) {
    logger.warn("Feedforward scaling enabled, but not applied.");
  }
  output = P + I + D + params.ff;
  output =
      std::clamp(output, params.constraints.first, params.constraints.second);
  return Controller::getOutput(); // Use getOutput() logging purposes.
};

double PID::getOutput(const double state, const double reference) {
  const double error{reference - state};
  const double P{params.kP * error};
  updateI(error);
  const double D{params.kD * (prevState - state)};
  prevState = state;
  if(params.ffScaling) {
    output = P + I + D + params.ff * reference;
  } else {
    output = P + I + D + params.ff;
  }
  output =
      std::clamp(output, params.constraints.first, params.constraints.second);
  return Controller::getOutput(); // Use getOutput() for logging purposes.
};

void PID::reset() {
  I = 0;
  prevState = 0;
  prevError = 0;
}

PID::Parameters PID::getParams() const {
  return params;
}

void PID::updateI(const double error) {
  if(std::abs(error) <= params.threshI) {
    I += params.kI * error;
  } else {
    I = 0;
  }
  if(std::signbit(error) != std::signbit(prevError)) {
    I = 0.0;
  }
  prevError = error;
  I = std::clamp(I, params.constraints.first, params.constraints.second);
}
}; // namespace atum