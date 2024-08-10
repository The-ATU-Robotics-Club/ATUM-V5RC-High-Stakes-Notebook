#include "pid.hpp"

namespace atum {
PID::PID(const Parameters &iParams) : params{iParams} {};

double PID::getOutput(const double error) {
  const double P{params.kP * error};
  if(std::abs(error) <= params.threshI)
    I += params.kI * error;
  else
    I = 0;
  if(std::signbit(error) != std::signbit(prevError)) I = 0.0;
  I = std::clamp(I, params.constraints.first, params.constraints.second);
  const double D{params.kD * (error - prevError)};
  prevError = error;
  output = P + I + D + params.ff;
  output =
      std::clamp(output, params.constraints.first, params.constraints.second);

  return output;
};

double PID::getOutput(const double state, const double reference) {
  const double error{reference - state};
  const double P{params.kP * error};
  if(std::abs(error) <= params.threshI)
    I += params.kI * error;
  else
    I = 0;
  if(std::signbit(error) != std::signbit(prevError)) I = 0.0;
  prevError = error;
  I = std::clamp(I, params.constraints.first, params.constraints.second);
  const double D{params.kD * (prevState - state)};
  prevState = state;
  output = P + I + D + params.ff * reference;
  output =
      std::clamp(output, params.constraints.first, params.constraints.second);
  return output;
};

void PID::reset() {
  I = 0;
  prevState = 0;
  prevError = 0;
}

PID::Parameters PID::getParams() const {
  return params;
}
}; // namespace atum