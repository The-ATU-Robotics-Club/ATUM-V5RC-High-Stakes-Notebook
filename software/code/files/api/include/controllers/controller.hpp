#pragma once

namespace atum {
class Controller {
  public:
  virtual double getOutput() const;

  virtual double getOutput(const double error) = 0;

  virtual double getOutput(const double state, const double reference) = 0;

  virtual void reset() = 0;

  protected:
  double output;
};
} // namespace atum