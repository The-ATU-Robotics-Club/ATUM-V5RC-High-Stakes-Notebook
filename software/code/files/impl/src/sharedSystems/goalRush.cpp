#include "goalRush.hpp"


namespace atum {

GoalRush::GoalRush(std::unique_ptr<Piston> iArm,
                   std::unique_ptr<Piston> iClamp,
                   std::unique_ptr<LimitSwitch> iLimitSwitch,
                   const Logger::Level loggerLevel) :
    arm{std::move(iArm)},
    clamp{std::move(iClamp)},
    limitSwitch{std::move(iLimitSwitch)},
    logger{loggerLevel} {
  logger.info("Goal rush is constructed!");
}

void GoalRush::extendArm() {
  arm->extend();
}

void GoalRush::retractArm() {
  arm->retract();
}

void GoalRush::toggleArm() {
  arm->toggle();
}

void GoalRush::grab() {
  clamp->retract();
}

void GoalRush::release() {
  clamp->extend();
}

void GoalRush::toggleClamp() {
  clamp->toggle();
}

bool GoalRush::isClamped() const {
  return !clamp->isExtended();
}

bool GoalRush::hasGoal() const {
  return limitSwitch->isPressed();
}
} // namespace atum