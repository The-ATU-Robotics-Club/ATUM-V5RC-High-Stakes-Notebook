#include "graph.hpp"

namespace atum {
namespace GUI {
const int Graph::graphResolution{10000};

void Graph::addValue(double value, const SeriesColor seriesColor) {
  // Don't plot anything if disabled (useful for testing)!
  if(pros::competition::is_disabled()) {
    return;
  }
  lv_chart_series_t *series{graphSeries[seriesColor]};
  const SeriesRange range{graphSeriesRanges[seriesColor]};
  const double absMin{std::abs(range.minimum)};
  value = absMin + std::clamp(value, range.minimum, range.maximum);
  const double rangeSum{absMin + std::abs(range.maximum)};
  value = value / rangeSum * 2 * graphResolution - graphResolution;
  lv_chart_set_next_value(graphChart, series, static_cast<lv_coord_t>(value));
  lv_chart_refresh(graphChart);
}

void Graph::setSeriesRange(const SeriesRange &range,
                           const SeriesColor seriesColor) {
  graphSeriesRanges[seriesColor] = range;
}

void Graph::setSeriesRange(double range, const SeriesColor seriesColor) {
  range = std::abs(range);
  graphSeriesRanges[seriesColor] = {-range, range};
}

void Graph::clearSeries(const SeriesColor seriesColor) {
  lv_chart_series_t *series{graphSeries[seriesColor]};
  lv_chart_set_all_value(graphChart, series, LV_CHART_POINT_NONE);
}

void Graph::clearAll() {
  clearSeries(SeriesColor::Red);
  clearSeries(SeriesColor::Green);
  clearSeries(SeriesColor::Blue);
  clearSeries(SeriesColor::Cyan);
  clearSeries(SeriesColor::Magenta);
  clearSeries(SeriesColor::Yellow);
  clearSeries(SeriesColor::White);
}

void Graph::setupScreen() {
  createLabel(graphScreen,
              "GRAPH",
              {250, defaultHeight},
              {defaultPadding, defaultPadding},
              {&rightBorder, &styleTitle});

  createScreenChangeButton(
      graphScreen,
      LV_SYMBOL_NEW_LINE,
      {150, defaultHeight},
      {-defaultPadding, defaultPadding, LV_ALIGN_TOP_RIGHT},
      mainMenuScreen,
      {&leftBorder});

  graphChart = lv_chart_create(graphScreen);
  lv_chart_set_type(graphChart, LV_CHART_TYPE_LINE);
  lv_obj_set_size(graphChart, fillWidth, fillHeight);
  lv_obj_align(graphChart, LV_ALIGN_TOP_MID, 0, contentYOffset);
  lv_chart_set_range(
      graphChart, LV_CHART_AXIS_PRIMARY_Y, -graphResolution, graphResolution);
  lv_chart_set_div_line_count(graphChart, 9, 18);
  lv_obj_add_style(graphChart, &styleChart, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_chart_set_point_count(graphChart, 500);
  graphSeries[SeriesColor::Red] =
      lv_chart_add_series(graphChart, red, LV_CHART_AXIS_PRIMARY_Y);
  graphSeries[SeriesColor::Green] =
      lv_chart_add_series(graphChart, green, LV_CHART_AXIS_PRIMARY_Y);
  graphSeries[SeriesColor::Blue] =
      lv_chart_add_series(graphChart, blue, LV_CHART_AXIS_PRIMARY_Y);
  graphSeries[SeriesColor::Cyan] =
      lv_chart_add_series(graphChart, cyan, LV_CHART_AXIS_PRIMARY_Y);
  graphSeries[SeriesColor::Magenta] =
      lv_chart_add_series(graphChart, magenta, LV_CHART_AXIS_PRIMARY_Y);
  graphSeries[SeriesColor::Yellow] =
      lv_chart_add_series(graphChart, yellow, LV_CHART_AXIS_PRIMARY_Y);
  graphSeries[SeriesColor::White] =
      lv_chart_add_series(graphChart, white, LV_CHART_AXIS_PRIMARY_Y);
}

lv_obj_t *Graph::graphChart;
std::array<lv_chart_series_t *, 7> Graph::graphSeries;
std::array<Graph::SeriesRange, 7> Graph::graphSeriesRanges{
    Graph::SeriesRange{graphResolution, graphResolution},
    Graph::SeriesRange{graphResolution, graphResolution},
    Graph::SeriesRange{graphResolution, graphResolution},
    Graph::SeriesRange{graphResolution, graphResolution},
    Graph::SeriesRange{graphResolution, graphResolution},
    Graph::SeriesRange{graphResolution, graphResolution},
    Graph::SeriesRange{graphResolution, graphResolution}};
} // namespace GUI
} // namespace atum