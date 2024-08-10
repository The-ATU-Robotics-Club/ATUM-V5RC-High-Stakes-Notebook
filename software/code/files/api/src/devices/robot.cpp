#include "robot.hpp"

namespace atum {
Robot::Robot(std::unique_ptr<Logger> iLogger) :
    Task{{"Robot"}}, logger{std::move(iLogger)} {
  start();
}

void Robot::schedule(const ScheduledAction &scheduledAction) {
  if(logger) logger->debug("Scheduled " + scheduledAction.name + "!");
  timedScheduledAction = make_pair(time(), scheduledAction);
}

void Robot::deschedule() {
  if(logger && timedScheduledAction)
    logger->debug("Descheduled " + timedScheduledAction.value().second.name +
                  "!");
  timedScheduledAction = {};
}

void Robot::taskFn1() {
  while(true) {
    wait(50_ms);
    if(!timedScheduledAction) continue;
    auto [startTime, scheduledAction] = timedScheduledAction.value();
    if(scheduledAction.timeout &&
       time() - startTime > scheduledAction.timeout) {
      if(scheduledAction.actOnTimeout && !pros::competition::is_disabled())
        scheduledAction.action();
      deschedule();
      if(logger)
        logger->warn("Scheduled action" + scheduledAction.name + " timed out!");
      continue;
    }
    if(scheduledAction.condition() && !pros::competition::is_disabled()) {
      scheduledAction.action();
      deschedule();
      continue;
    }
  }
}
} // namespace atum