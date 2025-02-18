/**
 * @file stateMachine.hpp
 * @brief Includes the StateMachine template class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../time/timer.hpp"

namespace atum {
/**
 * @brief Provides some common functionality associated with state machines.
 * Most is left to its derivatives, though, since much is subject to change
 * state machine to state machine.
 *
 * @tparam State
 */
template <typename State>
class StateMachine {
  public:
  /**
   * @brief Gets the current state.
   *
   * @return State
   */
  State getState() const {
    return state;
  }

  /**
   * @brief A condition to be used for scheduling or waiting. Returns a function
   * that, when called, returns true if the state is the same as the desired
   * state.
   *
   * @param desired
   * @return Condition
   */
  Condition checkStateIs(const State desired) {
    return [=]() { return state == desired; };
  }

  protected:
  State state;
};
} // namespace atum