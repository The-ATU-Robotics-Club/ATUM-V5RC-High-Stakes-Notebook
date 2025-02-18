#include "goalClamp.hpp"

namespace atum {
GoalClamp::GoalClamp(std::unique_ptr<Piston> iPiston,
                     std::unique_ptr<LimitSwitch> iLimitSwitch1,
                     std::unique_ptr<LimitSwitch> iLimitSwitch2,
                     const Logger::Level loggerLevel) :
    piston{std::move(iPiston)},
    limitSwitch1{std::move(iLimitSwitch1)},
    limitSwitch2{std::move(iLimitSwitch2)},
    logger{loggerLevel} {
  logger.info("Goal clamp is constructed!");
}

bool GoalClamp::hasGoal() {
  if(limitSwitch1 && limitSwitch2) {
    return limitSwitch1->isPressed() && limitSwitch2->isPressed();
  } else if(limitSwitch1) {
    return limitSwitch1->isPressed();
  } else if(limitSwitch2) {
    return limitSwitch2->isPressed();
  }
  return false;
}

bool GoalClamp::isClamped() {
  return piston->isExtended();
}

void GoalClamp::clamp() {
  piston->extend();
}

void GoalClamp::unclamp() {
  piston->retract();
}

void GoalClamp::toggleClamp() {
  piston->toggle();
}

Condition GoalClamp::checkHasGoal() {
  return [=]() { return hasGoal(); };
}
} // namespace atum