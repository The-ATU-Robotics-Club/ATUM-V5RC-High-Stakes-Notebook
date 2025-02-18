#include "map.hpp"

namespace atum {
namespace GUI {
const int Map::mapResolution{6000};

void Map::addPositions(const std::vector<Pose> &positions,
                       const SeriesColor seriesColor) {
  for(Pose position : positions) {
    addPosition(position, seriesColor);
  }
}

void Map::addPosition(const Pose position, const SeriesColor seriesColor) {
  lv_chart_series_t *series{mapSeries[seriesColor]};
  // From 12 in for the 6 ft in either direction from the origin.
  const int maxPossibleCoordinate{72};
  const int coordAdjustment{mapResolution / maxPossibleCoordinate};
  const lv_coord_t x{getValueAs<inch_t, lv_coord_t>(position.x) *
                     coordAdjustment};
  const lv_coord_t y{getValueAs<inch_t, lv_coord_t>(position.y) *
                     coordAdjustment};
  lv_chart_set_next_value2(mapChart, series, x, y);
  lv_chart_refresh(mapChart);
}

void Map::clearSeries(const SeriesColor seriesColor) {
  lv_chart_series_t *series{mapSeries[seriesColor]};
  lv_chart_set_all_value(mapChart, series, LV_CHART_POINT_NONE);
}

void Map::setupScreen() {
  lv_obj_t *mapLabel{
      createLabel(mapScreen,
                  "MAP",
                  {150, defaultHeight},
                  {-6 * defaultPadding, -70, LV_ALIGN_BOTTOM_RIGHT},
                  {&leftBorder, &styleTitle})};
  lv_obj_set_style_border_side(
      mapLabel,
      static_cast<lv_border_side_t>(LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT),
      LV_STATE_DEFAULT);

  createScreenChangeButton(mapScreen,
                           LV_SYMBOL_NEW_LINE,
                           {150, defaultHeight},
                           {-6 * defaultPadding, 70, LV_ALIGN_TOP_RIGHT},
                           mainMenuScreen,
                           {&leftBorder});

  mapChart = lv_chart_create(mapScreen);
  lv_chart_set_type(mapChart, LV_CHART_TYPE_SCATTER);
  static const int mapChartSize{screenHeight - 4 * defaultPadding};
  lv_obj_set_size(mapChart, mapChartSize, mapChartSize);
  lv_obj_align(mapChart, LV_ALIGN_TOP_LEFT, 6 * defaultPadding, defaultPadding);
  lv_chart_set_range(
      mapChart, LV_CHART_AXIS_PRIMARY_X, -mapResolution, mapResolution);
  lv_chart_set_range(
      mapChart, LV_CHART_AXIS_PRIMARY_Y, -mapResolution, mapResolution);
  lv_chart_set_point_count(mapChart, 30);
  lv_chart_set_div_line_count(mapChart, 7, 7);
  lv_obj_set_style_pad_all(mapChart, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(mapChart, 0, LV_PART_MAIN);
  lv_obj_set_style_line_width(mapChart, 0, LV_PART_ITEMS);
  lv_obj_set_style_size(mapChart, 12, LV_PART_INDICATOR);
  lv_obj_set_style_bg_img_src(mapChart, &field, LV_PART_MAIN);
  lv_obj_set_style_bg_img_opa(mapChart, LV_OPA_50, LV_PART_MAIN);
  lv_obj_add_style(mapChart, &styleChart, LV_PART_MAIN | LV_STATE_DEFAULT);
  mapSeries[SeriesColor::Red] =
      lv_chart_add_series(mapChart, red, LV_CHART_AXIS_PRIMARY_Y);
  mapSeries[SeriesColor::Green] =
      lv_chart_add_series(mapChart, green, LV_CHART_AXIS_PRIMARY_Y);
  mapSeries[SeriesColor::Blue] =
      lv_chart_add_series(mapChart, blue, LV_CHART_AXIS_PRIMARY_Y);
  mapSeries[SeriesColor::Cyan] =
      lv_chart_add_series(mapChart, cyan, LV_CHART_AXIS_PRIMARY_Y);
  mapSeries[SeriesColor::Magenta] =
      lv_chart_add_series(mapChart, magenta, LV_CHART_AXIS_PRIMARY_Y);
  mapSeries[SeriesColor::Yellow] =
      lv_chart_add_series(mapChart, yellow, LV_CHART_AXIS_PRIMARY_Y);
  mapSeries[SeriesColor::White] =
      lv_chart_add_series(mapChart, white, LV_CHART_AXIS_PRIMARY_Y);
}

lv_obj_t *Map::mapChart;
std::array<lv_chart_series_t *, 7> Map::mapSeries;
} // namespace GUI
} // namespace atum