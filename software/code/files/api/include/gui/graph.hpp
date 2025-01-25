/**
 * @file graph.hpp
 * @brief Includes the Graph class.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "../../pros/misc.hpp"
#include "screen.hpp"
#include <algorithm>
#include <array>

namespace atum {
namespace GUI {
/**
 * @brief This class encapsulates the logic and construction of the graphing
 * screen of the GUI. This is helpful for tuning motion profiles, controllers,
 * and general troubleshooting.
 *
 */
class Graph : public Screen {
  public:
  // Friend Manager to give access to screen set up. Not ideal, but
  // straightforward solution.
  friend class Manager;

  /**
   * @brief Packs together into named fields the minimum and maximum values
   * expected when graphing for a series.
   *
   */
  struct SeriesRange {
    double minimum;
    double maximum;
  };

  /**
   * @brief Adds a data point on one of the graph series. Won't add the point if
   * the robot is disabled (in order to provide the option to "freeze" data
   * input).
   *
   * @param value
   * @param seriesColor
   */
  static void addValue(double value, const SeriesColor seriesColor);

  /**
   * @brief Sets the range (negative and positive bounds) for one of the series.
   *
   * @param range
   * @param seriesColor
   */
  static void setSeriesRange(const SeriesRange &range,
                             const SeriesColor seriesColor);

  /**
   * @brief Sets the range for one of the series (in the form of [-range,
   * range]).
   *
   * @param range
   * @param seriesColor
   */
  static void setSeriesRange(double range, const SeriesColor seriesColor);

  /**
   * @brief Clears all the data points in a given series.
   *
   * @param seriesColor
   */
  static void clearSeries(const SeriesColor seriesColor);

  /**
   * @brief Clears all the data points in every series.
   *
   */
  static void clearAll();

  private:
  /**
   * @brief This deals with setting up the actual screen. Private to force
   * the proper series of steps for setup.
   *
   */
  static void setupScreen();

  static const int graphResolution;
  static lv_obj_t *graphChart;
  static std::array<lv_chart_series_t *, 7> graphSeries;
  static std::array<SeriesRange, 7> graphSeriesRanges;
};
} // namespace GUI
} // namespace atum