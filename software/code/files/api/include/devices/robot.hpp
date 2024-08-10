#pragma once

#include "../drive/drive.hpp"
#include "../time/task.hpp"

namespace atum {
class Robot : public Task {
  public:
  Robot(std::unique_ptr<Logger> iLogger);

  virtual void disabled() = 0;

  virtual void opcontrol() = 0;

  virtual void autonomous() = 0;

  using Condition = std::function<bool()>;
  using Action = std::function<void()>;
  struct ScheduledAction {
    Condition condition;
    Action action;
    std::string name;
    second_t timeout{0_s};
    bool actOnTimeout{true};
  };

  void schedule(const ScheduledAction &scheduledAction);

  void deschedule();

  virtual ~Robot() = default;

  protected:
  std::unique_ptr<Logger> logger;

  private:
  void taskFn1() override;

  std::optional<std::pair<second_t, ScheduledAction>> timedScheduledAction;
};

} // namespace atum