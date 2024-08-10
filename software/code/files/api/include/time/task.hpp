#pragma once

#include "api.h"
#include "time.hpp"

namespace atum {
class Task {
  public:
  struct TaskParams {
    std::string name{""};
    std::uint32_t priority{TASK_PRIORITY_DEFAULT};
    std::uint16_t depth{TASK_STACK_DEPTH_DEFAULT};
  };

  Task(const TaskParams &iParams1,
       const std::optional<TaskParams> &iParams2 = {},
       const std::optional<TaskParams> &iParams3 = {});

  void start();

  void stop();

  private:
  void createTask1();
  void createTask2();
  void createTask3();

  // Must implement one; if you need four, something's gone awry.
  virtual void taskFn1() = 0;
  virtual void taskFn2();
  virtual void taskFn3();

  std::unique_ptr<pros::Task> task1;
  std::unique_ptr<pros::Task> task2;
  std::unique_ptr<pros::Task> task3;
  const TaskParams params1;
  const std::optional<TaskParams> params2;
  const std::optional<TaskParams> params3;
};
} // namespace atum