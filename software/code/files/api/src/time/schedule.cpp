#include "schedule.hpp"


namespace atum {
#define SCHEDULE()

const std::function<void()> Schedule::doNothing{[]() {}};
const Condition Schedule::neverMet{[]() { return false; }};

Schedule::Schedule(const Item &iItem, const Logger::Level loggerLevel) :
    Task{this, loggerLevel},
    item{iItem},
    logger{loggerLevel} {
  startBackgroundTasks();
  logger.debug("The item \"" + item.name + "\" has been scheduled.");
}

Schedule::~Schedule() {
  stopBackgroundTasks();
  logger.debug("Scheduled item was interrupted (out of scope).");
}

TASK_DEFINITIONS_FOR(Schedule) {
  START_TASK("Scheduled Item")
  const uint8_t initialStatus{pros::competition::get_status()};
  Timer timer{item.timeout};
  try {
    while(pros::competition::get_status() == initialStatus &&
          !timer.goneOff() && !item.condition()) {
      // Higher than standard delay to allow several scheduled items at once
      // with little impact.
      wait(100_ms);
    }
  } catch(...) {
    logger.warn("Scheduled item reference out of scope!");
  }
  if(pros::competition::get_status() != initialStatus) {
    logger.debug("Scheduled item was interrupted (status change).");
    return;
  }
  if(item.timeout && timer.goneOff()) {
    item.todoTimeout();
    logger.debug("The scheduled item \"" + item.name + "\" has timed out.");
    return;
  }
  item.todo();
  logger.debug("The scheduled item \"" + item.name + "\" is finished.");
  END_TASK
}
} // namespace atum