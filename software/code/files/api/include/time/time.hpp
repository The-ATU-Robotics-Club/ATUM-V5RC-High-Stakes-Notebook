/**
 * @file time.hpp
 * @brief Includes various functions, constants, and aliases related to time.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../utility/units.hpp"
#include "api.h"

namespace atum {
/**
 * @brief Alias provided for the very common function type returning a boolean
 * value.
 *
 */
using Condition = std::function<bool()>;

/**
 * @brief Because many analog ADI sensors have an option to calibrate and that
 * calibration involves 500 samples taken 1 ms apart, this constant is for use
 * in delays after said calibration begins.
 *
 */
static constexpr second_t adiCalibrationTime{500_ms};

/**
 * @brief This is the typical rate at which devices can give or receive data.
 *
 */
static constexpr second_t standardDelay{10_ms};

/**
 * @brief This is a length of time considered to be "forever" for the sake of
 * delays.
 *
 */
static constexpr hour_t forever{infinite};

/**
 * @brief Gets the current time since starting.
 *
 * @return second_t
 */
second_t time();

/**
 * @brief Waits for the specified amount of time (or standard delay of 10 ms if
 * no such time is given).
 *
 * @param delay
 */
void wait(second_t delay = standardDelay);

/**
 * @brief Waits until the condition given is true or the timeout is reached
 * (unless forever is provided for the timeout). The delay parameter refers to
 * how long to wait between each check, provided for sensors like the color
 * sensor that benefit from specific delay.
 *
 * @param condition
 * @param timeout
 * @param delay
 */
void waitUntil(const Condition &condition,
               const second_t timeout = forever,
               const second_t delay = standardDelay);
} // namespace atum