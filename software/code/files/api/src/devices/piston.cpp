#include "piston.hpp"

namespace atum {
Piston::Piston(const std::uint8_t port,
               const bool iReversed,
               const bool startExtended,
               const Logger::Level loggerLevel) :
    piston{port, iReversed ? !startExtended : startExtended},
    reversed{iReversed},
    logger{loggerLevel} {
  logger.debug("Piston on port " +
               std::to_string(std::get<1>(piston.get_port())) +
               " has been constructed.");
}

Piston::Piston(const ADIExtenderPort &port,
               const bool iReversed,
               const bool startExtended,
               const Logger::Level loggerLevel) :
    piston{port(), iReversed ? !startExtended : startExtended},
    reversed{iReversed},
    logger{loggerLevel} {
  logger.debug("Piston on port " +
               std::to_string(std::get<1>(piston.get_port())) +
               " has been constructed.");
}

void Piston::extend() {
  if(reversed) {
    piston.retract();
  } else {
    piston.extend();
  }
}

void Piston::retract() {
  if(reversed) {
    piston.extend();
  } else {
    piston.retract();
  }
}

void Piston::toggle() {
  piston.toggle();
}

bool Piston::isExtended() {
  if(reversed) {
    return !piston.is_extended();
  }
  return piston.is_extended();
}
} // namespace atum