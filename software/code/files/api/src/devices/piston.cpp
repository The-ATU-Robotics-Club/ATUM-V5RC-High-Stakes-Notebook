#include "piston.hpp"

namespace atum {
Piston::Piston(const std::uint8_t port) : piston{port} {}

Piston::Piston(const pros::ext_adi_port_pair_t port) : piston{port} {}

void Piston::toggle() {
  if(extended)
    retract();
  else
    extend();
}

void Piston::extend() {
  piston.set_value(1);
  extended = true;
}

void Piston::retract() {
  piston.set_value(0);
  extended = false;
}

bool Piston::isOut() {
  return extended;
}
} // namespace atum