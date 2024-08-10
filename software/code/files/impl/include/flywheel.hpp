#pragma once

#include "atum/atum.hpp"

namespace atum {
class Flywheel : public Task {
  public:
  Flywheel(std::unique_ptr<pros::MotorGroup> iWheel,
           std::initializer_list<double> iPresets,
           std::unique_ptr<Controller> iController,
           std::unique_ptr<Logger> iLogger = nullptr);
           
  void setSpecificSpeed(const double iReference);
  
  void setSpeed(const std::size_t referenceIndex);

  void cycleSpeed();
  
  void increaseSpeed();
  
  void decreaseSpeed();
  
  double getVelocity() const;

  double getReference() const;

  private:
  void taskFn1() override;

  std::unique_ptr<pros::MotorGroup> wheel;
  const std::vector<double> presets;
  std::unique_ptr<Controller> controller;
  std::unique_ptr<Logger> logger;
  int referenceIndex{0};
  double reference{0};
};
} // namespace atum