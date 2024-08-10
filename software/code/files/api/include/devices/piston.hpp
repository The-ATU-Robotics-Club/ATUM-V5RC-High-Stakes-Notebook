#pragma once

#include "api.h"

namespace atum {
class Piston {
  public:
  explicit Piston(const std::uint8_t port);
  
  explicit Piston(const pros::ext_adi_port_pair_t port);
  
  void toggle();
  
  void extend();
  
  void retract();
  
  bool isOut();

  private:
  pros::ADIDigitalOut piston;
  bool extended{false};
};
} // namespace atum