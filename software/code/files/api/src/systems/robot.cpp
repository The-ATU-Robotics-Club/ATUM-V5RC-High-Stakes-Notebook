#include "robot.hpp"

namespace atum {
void Robot::autonomous() {
  const std::size_t routineIndex{GUI::Routines::selectedRoutine()};
  routines[routineIndex]();
}

std::string Robot::getRoutineNames() {
  return routineNames;
}
} // namespace atum