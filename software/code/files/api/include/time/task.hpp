/**
 * @file task.hpp
 * @brief Includes the Task parent class as well as the macros needed for it to
 * function.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../utility/logger.hpp"
#include "../utility/misc.hpp"
#include "api.h"

namespace atum {
/**
 * @brief Automatically inserts boilerplate to make use of the Task class.
 * Should be placed on the line immediately following a derivative of Task's
 * opening curly brace for its definition.
 *
 */
#define TASK_BOILERPLATE()                                                     \
  public:                                                                      \
  friend class Task;                                                           \
                                                                               \
  protected:                                                                   \
  std::string getHandlerName() const;                                          \
  void prepBackgroundTasks()

/**
 * @brief Creates space within which to place the tasks the handler
 * will want to use.
 *
 */
#define TASK_DEFINITIONS_FOR(handler)                                          \
  std::string handler::getHandlerName() const {                                \
    return #handler;                                                           \
  }                                                                            \
  void handler::prepBackgroundTasks()

/**
 * @brief Start the definition of a task with just its name. Do not use
 * explicitly, ust START_TASK instead.
 *
 */
#define START_TASK_1(name) taskParams.push_back({name, TASK_PRIORITY_DEFAULT, [=](){
/**
 * @brief Start the definition of a task with its name and priority. Do not use
 * explicity, use START_TASK instead.
 *
 */
#define START_TASK_2(name, priority) taskParams.push_back({name, priority, [=](){
/**
 * @brief Starts the definition of a task. Accepts between one and two
 * parameters. First refers to the name of the task, second refers to its
 * priority.
 *
 */
#define START_TASK(...)                                                        \
  GET_MACRO(__VA_ARGS__, START_TASK_2, START_TASK_1)(__VA_ARGS__)

/**
 * @brief Ends a task definition.
 *
 */
#define END_TASK                                                               \
  }                                                                            \
  }                                                                            \
  );

/**
 * @brief This class allows its derivatives to "own" their own tasks and better
 * control their management. To use, publicly derive on the desired class and
 * use the TASK_BOILERPLATE macro. Then, in the cpp file for the class, begin
 * task definitions with "TASK_DEFINITIONS_FOR(<name of the class>) {",
 * "START_TASK(<name>, <priority (optional>))" to begin defining a task, and
 * "END_TASK" to cap the task definition.
 *
 * Essentially, this allows classes like "Flywheel" to have background loops for
 * things like velocity control.
 *
 */
class Task {
  public:
  // These are deleted to force the use of the templated constructor.
  Task() = delete;
  Task(const Task &) = delete;
  Task(Task &&) = delete;

  /**
   * @brief Constructs the Task by calling prepBackGroundTasks to essentially
   * store the task information to later be started proper.
   *
   * @tparam TaskHandler
   * @param handler
   * @param loggerLevel
   */
  template <class TaskHandler>
  Task(TaskHandler *handler,
       const Logger::Level loggerLevel = Logger::Level::Info) :
      handlerName{handler->getHandlerName()},
      taskLogger{loggerLevel} {
    handler->prepBackgroundTasks();
    taskLogger.debug("Tasks associated with " + handlerName +
                     " have been prepared to start.");
  }

  /**
   * @brief Start the background tasks if they haven't already started.
   *
   */
  void startBackgroundTasks();

  /**
   * @brief Stop the background tasks.
   *
   */
  void stopBackgroundTasks();

  protected:
  /**
   * @brief The information needed to start a task for internal use.
   *
   */
  struct TaskParams {
    const std::string name;
    const std::uint32_t priority;
    const std::function<void()> taskFn;
  };

  std::vector<TaskParams> taskParams;
  const std::string handlerName;

  private:
  std::vector<std::unique_ptr<pros::Task>> tasks;
  Logger taskLogger; // Named differently than normal for simple disambiguation.
};
} // namespace atum