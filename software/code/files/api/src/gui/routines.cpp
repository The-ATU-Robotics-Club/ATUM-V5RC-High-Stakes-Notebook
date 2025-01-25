#include "routines.hpp"

namespace atum {
namespace GUI {
std::size_t Routines::selectedRoutine() {
  return lv_dropdown_get_selected(routineSelections);
}

MatchColor Routines::selectedColor() {
  return static_cast<MatchColor>(
      lv_obj_has_state(colorSwitch, LV_STATE_CHECKED));
}

void Routines::setupScreen(const std::string routines) {
  createLabel(routinesScreen,
              "ROUTINES",
              {250, defaultHeight},
              {defaultPadding, defaultPadding},
              {&rightBorder, &styleTitle});

  createScreenChangeButton(
      routinesScreen,
      LV_SYMBOL_NEW_LINE,
      {150, defaultHeight},
      {-defaultPadding, defaultPadding, LV_ALIGN_TOP_RIGHT},
      mainMenuScreen,
      {&leftBorder});

  routineSelections = lv_dropdown_create(routinesScreen);
  lv_dropdown_set_options(routineSelections, routines.c_str());
  lv_obj_align(routineSelections, LV_ALIGN_TOP_MID, 0, contentYOffset);
  lv_obj_set_size(routineSelections, fillWidth, defaultHeight);
  lv_obj_t *routineSelectionsList{lv_dropdown_get_list(routineSelections)};
  lv_obj_set_style_max_height(
      routineSelectionsList, fillHeight - defaultHeight, LV_STATE_DEFAULT);
  lv_obj_add_style(routineSelections, &styleDropDownIdle, LV_STATE_DEFAULT);
  lv_obj_add_style(routineSelections, &styleDropDownPressed, LV_STATE_PRESSED);
  lv_obj_add_style(routineSelectionsList, &styleDropDownIdle, LV_STATE_DEFAULT);
  lv_obj_add_style(routineSelectionsList,
                   &styleDropDownPressed,
                   LV_PART_SELECTED | LV_STATE_PRESSED);
  lv_obj_set_style_bg_color(
      routineSelectionsList, darkGrey, LV_PART_SELECTED | LV_STATE_CHECKED);

  colorSwitch = lv_switch_create(routinesScreen);
  lv_obj_set_size(colorSwitch, fillWidth, 3 * defaultHeight);
  lv_obj_align(colorSwitch, LV_ALIGN_BOTTOM_MID, 0, -defaultPadding);
  lv_obj_set_style_bg_color(colorSwitch, red, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(colorSwitch, white, LV_PART_KNOB);
  lv_obj_set_style_bg_color(
      colorSwitch, blue, LV_PART_INDICATOR | LV_STATE_CHECKED);
}

lv_obj_t *Routines::routineSelections;
lv_obj_t *Routines::colorSwitch;
} // namespace GUI
} // namespace atum
