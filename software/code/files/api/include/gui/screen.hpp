/**
 * @file screen.hpp
 * @brief Includes the Screen class and the available series colors.
 * @date 2024-12-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "imgs/imgs.hpp"
#include "liblvgl/lvgl.h"
#include <string>
#include <vector>

namespace atum {
namespace GUI {
/**
 * @brief This enumerated value represents the different series available to
 * write to on the map and graph screens.
 *
 */
enum SeriesColor { Red, Green, Blue, Cyan, Magenta, Yellow, White };

/**
 * @brief This class provides the common resources and methods needed by several
 * different screens in a more encapsulated fasion.
 *
 */
class Screen {
  protected:
  /**
   * @brief Packs together the width and height of GUI elements.
   *
   */
  struct GUISize {
    const int width;
    const int height;
  };

  /**
   * @brief Packs together the x offset, y offset, and alignment of GUI
   * elements.
   *
   */
  struct GUIPosition {
    const int x;
    const int y;
    const lv_align_t align{LV_ALIGN_TOP_LEFT};
  };

  /**
   * @brief Allocates space for all of the screens. Should be ran early in
   * initialization.
   *
   */
  static void createScreens();

  /**
   * @brief Initializes the various styles used across the GUI. Should be ran
   * early in initialization.
   *
   */
  static void initializeStyles();

  /**
   * @brief Adds several styles to an object at once.
   *
   * @param obj
   * @param styles
   * @param selector
   */
  static void addStyles(lv_obj_t *obj,
                        const std::vector<lv_style_t *> &styles,
                        const lv_style_selector_t selector = LV_STATE_DEFAULT);

  /**
   * @brief The callback used whenever a button or element being interacted with
   * should result in the screen being changed.
   *
   * @param event
   */
  static void changeScreenCB(lv_event_t *event);

  /**
   * @brief Creates a button to change the screen.
   *
   * @param currentScr
   * @param text
   * @param size
   * @param position
   * @param nextScr
   * @param extraIdleStyles
   * @param extraPressedStyles
   * @return lv_obj_t*
   */
  static lv_obj_t *createScreenChangeButton(
      lv_obj_t *currentScr,
      const std::string &text,
      const GUISize &size,
      const GUIPosition &position,
      lv_obj_t *nextScr = nullptr,
      const std::vector<lv_style_t *> &extraIdleStyles = {},
      const std::vector<lv_style_t *> &extraPressedStyles = {});

  /**
   * @brief Creates a label. Typically used in the title for screens.
   *
   * @param scr
   * @param text
   * @param size
   * @param position
   * @param styles
   * @return lv_obj_t*
   */
  static lv_obj_t *createLabel(lv_obj_t *scr,
                               const std::string &text,
                               const GUISize &size,
                               const GUIPosition &position,
                               const std::vector<lv_style_t *> &styles = {});

  // Commonly used dimensions/lengths.
  static const int screenWidth;
  static const int screenHeight;
  static const int bgBorderWidth;
  static const int workingWidth;
  static const int workingHeight;
  static const int defaultHeight;
  static const int defaultPadding;
  static const int fillWidth;
  static const int fillHeight;
  static const int contentYOffset;

  // Commonly used colors.
  static const lv_color_t black;
  static const lv_color_t darkGrey;
  static const lv_color_t grey;
  static const lv_color_t lightGrey;
  static const lv_color_t red;
  static const lv_color_t green;
  static const lv_color_t blue;
  static const lv_color_t cyan;
  static const lv_color_t magenta;
  static const lv_color_t yellow;
  static const lv_color_t white;

  // Commonly used styles.
  static lv_style_t styleButtonIdle;
  static lv_style_t styleButtonPressed;
  static lv_style_t styleBG;
  static lv_style_t leftBorder;
  static lv_style_t rightBorder;
  static lv_style_t styleTitle;
  static lv_style_t styleDropDownIdle;
  static lv_style_t styleDropDownPressed;
  static lv_style_t styleChart;

  // Screens.
  static lv_obj_t *loadingScreen;
  static lv_obj_t *mainMenuScreen;
  static lv_obj_t *homeScreen;
  static lv_obj_t *routinesScreen;
  static lv_obj_t *logScreen;
  static lv_obj_t *graphScreen;
  static lv_obj_t *mapScreen;
};
} // namespace GUI
} // namespace atum