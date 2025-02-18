/**
 * @file map.hpp
 * @brief Includes the Map class. 
 * @date 2024-12-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../pose/pose.hpp"
#include "screen.hpp"
#include <array>

namespace atum {
namespace GUI {
/**
 * @brief This class encapsulates the logic and construction of the map
 * screen of the GUI. This is useful for visualizing pathing and tracking
 * systems.
 *
 */
class Map : public Screen {
  public:
  // Friend Manager to give access to screen set up. Not ideal, but
  // straightforward solution.
  friend class Manager;

  /**
   * @brief Add several points to a series on the map at once.
   *
   * @param positions
   * @param seriesColor
   */
  static void addPositions(const std::vector<Pose> &positions,
                           const SeriesColor seriesColor);

  /**
   * @brief Add a point to a series on the map.
   *
   * @param position
   * @param seriesColor
   */
  static void addPosition(const Pose position, const SeriesColor seriesColor);

  /**
   * @brief Clear all points in a series.
   *
   * @param seriesColor
   */
  static void clearSeries(const SeriesColor seriesColor);

  private:
  /**
   * @brief This deals with setting up the actual screen. Private to force the
   * proper series of steps for setup.
   *
   */
  static void setupScreen();

  static const int mapResolution;
  static lv_obj_t *mapChart;
  static std::array<lv_chart_series_t *, 7> mapSeries;
};
} // namespace GUI
} // namespace atum