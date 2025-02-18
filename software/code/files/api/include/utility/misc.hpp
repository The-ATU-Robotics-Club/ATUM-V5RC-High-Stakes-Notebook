/**
 * @file misc.hpp
 * @brief Includes various functions, aliases, and macros that are useful but
 * don't categorize well.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "api.h"
#include "pros/apix.h"
#include <fstream>
#include <numeric>

namespace atum {
/**
 * @brief Used internally to automatically make use of different macros
 * depending on number of parameters. Do not use explicity.
 *
 */
#define GET_MACRO(_1, _2, NAME, ...) NAME

/**
 * @brief Provided for disambiguation purposes in a few constructors.
 *
 */
using SmartPort = std::int8_t;

/**
 * @brief Because make_unique and make_shared seemed to brick the type inference
 * of constructors involving vectors, this aliases makes constructed smart
 * pointers to said objects a little nicer looking.
 *
 *
 */
using PortsList = std::vector<std::uint8_t>;

/**
 * @brief Same as PortsList, but for the signed integers associated with
 * reversed motors.
 *
 */
using MotorPortsList = std::vector<std::int8_t>;

/**
 * @brief Used in the case where a port can not be found during dynamic
 * initialization of a device.
 *
 */
static constexpr std::int8_t errorPort{0};

/**
 * @brief Smallest possible non-zero value for double. Typically used to avoid
 * division by zero errors.
 *
 */
static constexpr double infinitesimal{std::numeric_limits<double>::min()};

/**
 * @brief Largest possible value for double. Typically used to prevent
 * conditions from ever being met.
 *
 */
static constexpr double infinite{std::numeric_limits<double>::max()};

/**
 * @brief A simple templated function to take an average of a vector of items.
 *
 * @tparam T
 * @param items
 * @return T
 */
template <typename T>
T average(const std::vector<T> &items) {
  if(items.empty()) {
    return T{0};
  }
  const double size{static_cast<double>(items.size())};
  return std::reduce(items.begin(), items.end()) / size;
}
} // namespace atum