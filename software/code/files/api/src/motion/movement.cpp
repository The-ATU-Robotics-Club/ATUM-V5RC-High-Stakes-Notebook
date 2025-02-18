#include "movement.hpp"

namespace atum {
void Movement::interrupt() {
  interrupted = true;
}

void Movement::setFlipped(const bool iFlipped) {
  flipped = iFlipped;
}

bool Movement::flipped{false};
} // namespace atum