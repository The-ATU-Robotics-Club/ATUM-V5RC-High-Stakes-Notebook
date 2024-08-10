#include "flywheel.hpp"

namespace atum {
Flywheel::Flywheel(std::unique_ptr<pros::MotorGroup> iWheel,
                   std::initializer_list<double> iPresets,
                   std::unique_ptr<Controller> iController,
                   std::unique_ptr<Logger> iLogger) :
    Task({"Flywheel"}),
    wheel{std::move(iWheel)},
    presets{iPresets},
    controller{std::move(iController)},
    logger{std::move(iLogger)} {
  start();
  if(logger) logger->info("Flywheel constructed!");
}

void Flywheel::setSpecificSpeed(const double iReference) {
  reference = iReference;
}

void Flywheel::setSpeed(const std::size_t iReferenceIndex) {
  referenceIndex = iReferenceIndex;
  if(logger && referenceIndex >= presets.size())
    logger->warn("Reference index out of bounds!");
  // Clamp in case driver spams increase/decrease speed.
  referenceIndex =
      std::clamp(referenceIndex, 0, static_cast<int>(presets.size()) - 1);
  setSpecificSpeed(presets[referenceIndex]);
}

void Flywheel::cycleSpeed() {
  const std::size_t index{(referenceIndex + 1) % presets.size()};
  setSpeed(index);
}

void Flywheel::increaseSpeed() {
  setSpeed(referenceIndex + 1);
}

void Flywheel::decreaseSpeed() {
  setSpeed(referenceIndex - 1);
}

double Flywheel::getVelocity() const {
  return average(wheel->get_actual_velocities());
}

double Flywheel::getReference() const {
  return reference;
}

void Flywheel::taskFn1() {
  while(true) {
    if(logger)
      logger->debug("Flywheel Velocity: " + std::to_string(getVelocity()));
    if(reference) {
      controller->getOutput(getVelocity(), reference);
      wheel->move_voltage(controller->getOutput());
    } else
      wheel->move_voltage(0);
    wait(10_ms);
  }
}

} // namespace atum