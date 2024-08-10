#include "misc.hpp"

namespace atum {
std::unique_ptr<pros::Motor> createMotor(const std::int8_t port,
                                         const pros::motor_gearset_e gear) {
  std::unique_ptr<pros::Motor> motor{std::make_unique<pros::Motor>(port, gear)};
  motor->set_encoder_units(
      pros::motor_encoder_units_e_t::E_MOTOR_ENCODER_DEGREES);
  return motor;
}

std::unique_ptr<pros::MotorGroup>
    createMotorGroup(std::initializer_list<std::int8_t> ports,
                     const pros::motor_gearset_e gear) {
  std::unique_ptr<pros::MotorGroup> motors{
      std::make_unique<pros::MotorGroup>(ports)};
  motors->set_gearing(gear);
  motors->set_encoder_units(
      pros::motor_encoder_units_e_t::E_MOTOR_ENCODER_DEGREES);
  return motors;
}

bool fileExists(const std::string &filename) {
  std::ifstream file{filename};
  return file.good();
}
} // namespace atum