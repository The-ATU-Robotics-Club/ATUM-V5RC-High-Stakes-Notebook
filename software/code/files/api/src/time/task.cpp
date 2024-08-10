#include "task.hpp"

namespace atum {
Task::Task(const TaskParams &iParams1,
           const std::optional<TaskParams> &iParams2,
           const std::optional<TaskParams> &iParams3) :
    params1{iParams1}, params2{iParams2}, params3{iParams3} {}

void Task::start() {
  createTask1();
  if(params2) createTask2();
  if(params3) createTask3();
}

void Task::stop() {
  task1->remove();
  task1 = nullptr;
  task2->remove();
  task2 = nullptr;
  task3->remove();
  task3 = nullptr;
}

void Task::createTask1() {
  std::cout << __LINE__ << '\n';
  task1 = std::make_unique<pros::Task>([this]() { taskFn1(); },
                                       params1.priority,
                                       params1.depth,
                                       params1.name.c_str());
}

void Task::createTask2() {
  std::cout << __LINE__ << '\n';
  task2 = std::make_unique<pros::Task>([this]() { taskFn2(); },
                                       params2.value().priority,
                                       params2.value().depth,
                                       params2.value().name.c_str());
}

void Task::createTask3() {
  std::cout << __LINE__ << '\n';
  task3 = std::make_unique<pros::Task>([this]() { taskFn3(); },
                                       params3.value().priority,
                                       params3.value().depth,
                                       params3.value().name.c_str());
}

void Task::taskFn2() {}

void Task::taskFn3() {}
} // namespace atum