#include "controller.hpp"

namespace atum {
Controller::Controller(const Logger::Level loggerLevel) : logger{loggerLevel} {}

double Controller::getOutput() {
  logger.debug("Output of controller is " + std::to_string(output));
  return output;
}
} // namespace atum