#pragma once

#include "../utility/units.hpp"
#include "api.h"

namespace atum {
second_t time();

void wait(second_t time = 10_ms);

void waitUntil(const std::function<bool()> &condition, const second_t timeout = 0_s);
} // namespace atum