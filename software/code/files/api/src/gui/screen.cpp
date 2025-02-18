#include "screen.hpp"

namespace atum {
namespace GUI {
// Commonly used dimensions/lengths.
const int Screen::screenWidth{480};
const int Screen::screenHeight{240};
const int Screen::bgBorderWidth{5};
const int Screen::workingWidth{screenWidth - bgBorderWidth};
const int Screen::workingHeight{screenHeight - bgBorderWidth};
const int Screen::defaultHeight{40};
const int Screen::defaultPadding{bgBorderWidth};
const int Screen::fillWidth{workingWidth - 4 * defaultPadding};
const int Screen::fillHeight{workingHeight - defaultHeight -
                             4 * defaultPadding};
const int Screen::contentYOffset{defaultHeight + 2 * defaultPadding};

// Commonly used colors.
const lv_color_t Screen::black{lv_color_hex(0x1f1f1f)};
const lv_color_t Screen::darkGrey{lv_color_hex(0x303030)};
const lv_color_t Screen::grey{lv_color_hex(0x454545)};
const lv_color_t Screen::lightGrey{lv_color_hex(0xbebebe)};
const lv_color_t Screen::red{lv_color_hex(0xFF0000)};
const lv_color_t Screen::green{lv_color_hex(0x00FF00)};
const lv_color_t Screen::blue{lv_color_hex(0x0000FF)};
const lv_color_t Screen::cyan{lv_color_hex(0x00FFFF)};
const lv_color_t Screen::magenta{lv_color_hex(0xFF00FF)};
const lv_color_t Screen::yellow{lv_color_hex(0xFFFF00)};
const lv_color_t Screen::white{lv_color_white()};

void Screen::createScreens() {
  loadingScreen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(loadingScreen,
                            lv_color_black(),
                            LV_PART_MAIN); // Darker black than normal.
  lv_obj_set_style_bg_color(loadingScreen, black, LV_PART_MAIN);
  mainMenuScreen = lv_obj_create(nullptr);
  lv_obj_add_style(mainMenuScreen, &styleBG, 0);
  homeScreen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(homeScreen,
                            lv_color_black(),
                            LV_PART_MAIN); // Darker black than normal.
  lv_obj_add_event_cb(homeScreen,
                      changeScreenCB,
                      LV_EVENT_CLICKED,
                      static_cast<void *>(mainMenuScreen));
  routinesScreen = lv_obj_create(nullptr);
  lv_obj_add_style(routinesScreen, &styleBG, 0);
  logScreen = lv_obj_create(nullptr);
  lv_obj_add_style(logScreen, &styleBG, 0);
  graphScreen = lv_obj_create(nullptr);
  lv_obj_add_style(graphScreen, &styleBG, 0);
  mapScreen = lv_obj_create(nullptr);
  lv_obj_add_style(mapScreen, &styleBG, 0);
}

void Screen::initializeStyles() {
  // Background style.
  lv_style_init(&styleBG);
  lv_style_set_bg_color(&styleBG, grey);
  lv_style_set_bg_opa(&styleBG, LV_OPA_COVER);
  lv_style_set_border_opa(&styleBG, LV_OPA_COVER);
  lv_style_set_border_width(&styleBG, bgBorderWidth);
  lv_style_set_border_color(&styleBG, darkGrey);

  // Left border style.
  lv_style_init(&leftBorder);
  lv_style_set_border_side(&leftBorder,
                           static_cast<lv_border_side_t>(LV_BORDER_SIDE_BOTTOM |
                                                         LV_BORDER_SIDE_LEFT));
  lv_style_set_border_color(&leftBorder, white);
  lv_style_set_border_width(&leftBorder, 2);
  lv_style_set_radius(&leftBorder, 5);

  // Right border style.
  lv_style_init(&rightBorder);
  lv_style_set_border_side(&rightBorder,
                           static_cast<lv_border_side_t>(LV_BORDER_SIDE_BOTTOM |
                                                         LV_BORDER_SIDE_RIGHT));
  lv_style_set_border_color(&rightBorder, white);
  lv_style_set_border_width(&rightBorder, 2);
  lv_style_set_radius(&rightBorder, 5);

  // Title label style.
  lv_style_init(&styleTitle);
  lv_style_set_text_color(&styleTitle, black);
  lv_style_set_text_font(&styleTitle, &lv_font_montserrat_36);
  lv_style_set_text_align(&styleTitle, LV_TEXT_ALIGN_CENTER);
  lv_style_set_text_letter_space(&styleTitle, -4);
  lv_style_set_bg_color(&styleTitle, lightGrey);
  lv_style_set_bg_opa(&styleTitle, LV_OPA_COVER);

  // Idle button style.
  lv_style_init(&styleButtonIdle);
  lv_style_set_text_color(&styleButtonIdle, black);
  lv_style_set_text_font(&styleButtonIdle, &lv_font_montserrat_36);
  lv_style_set_text_letter_space(&styleButtonIdle, -4);
  lv_style_set_bg_color(&styleButtonIdle, lightGrey);
  lv_style_set_bg_opa(&styleButtonIdle, LV_OPA_COVER);
  lv_style_set_shadow_opa(&styleButtonIdle, LV_OPA_TRANSP);

  // Pressed button style.
  lv_style_init(&styleButtonPressed);
  lv_style_set_text_color(&styleButtonPressed, white);
  lv_style_set_bg_color(&styleButtonPressed, black);

  // Idle drop down style
  lv_style_init(&styleDropDownIdle);
  lv_style_set_text_color(&styleDropDownIdle, black);
  lv_style_set_text_font(&styleDropDownIdle, &lv_font_montserrat_20);
  lv_style_set_text_align(&styleDropDownIdle, LV_TEXT_ALIGN_CENTER);
  lv_style_set_bg_color(&styleDropDownIdle, lightGrey);
  lv_style_set_bg_opa(&styleDropDownIdle, LV_OPA_COVER);

  // Pressed drop down style.
  lv_style_init(&styleDropDownPressed);
  lv_style_set_text_color(&styleDropDownPressed, white);
  lv_style_set_bg_color(&styleDropDownPressed, black);

  // Chart style.
  lv_style_init(&styleChart);
  lv_style_set_bg_color(&styleChart, black);
}

void Screen::addStyles(lv_obj_t *obj,
                       const std::vector<lv_style_t *> &styles,
                       const lv_style_selector_t selector) {
  for(lv_style_t *style : styles) {
    lv_obj_add_style(obj, style, selector);
  }
}

void Screen::changeScreenCB(lv_event_t *event) {
  void *userData{lv_event_get_user_data(event)};
  lv_obj_t *scr{static_cast<lv_obj_t *>(userData)};
  if(scr) {
    lv_scr_load(scr);
  }
}

lv_obj_t *Screen::createScreenChangeButton(
    lv_obj_t *currentScr,
    const std::string &text,
    const GUISize &size,
    const GUIPosition &position,
    lv_obj_t *nextScr,
    const std::vector<lv_style_t *> &extraIdleStyles,
    const std::vector<lv_style_t *> &extraPressedStyles) {
  // Create the button, size it, and place it.
  lv_obj_t *button{lv_btn_create(currentScr)};
  lv_obj_set_size(button, size.width, size.height);
  lv_obj_align(button, position.align, position.x, position.y);

  // Add and place the text on the button.
  lv_obj_t *label{lv_label_create(button)};
  lv_label_set_text(label, text.c_str());
  lv_obj_center(label);

  // Add screen change callback with next screen as a parameter.
  lv_obj_add_event_cb(
      button, changeScreenCB, LV_EVENT_CLICKED, static_cast<void *>(nextScr));

  // Add styles.
  lv_obj_add_style(button, &styleButtonIdle, 0);
  addStyles(button, extraIdleStyles);
  lv_obj_add_style(button, &styleButtonPressed, LV_STATE_PRESSED);
  addStyles(button, extraPressedStyles, LV_STATE_PRESSED);

  return button;
}

lv_obj_t *Screen::createLabel(lv_obj_t *scr,
                              const std::string &text,
                              const GUISize &size,
                              const GUIPosition &position,
                              const std::vector<lv_style_t *> &styles) {
  lv_obj_t *label{lv_label_create(scr)};
  lv_label_set_text(label, text.c_str());
  lv_obj_set_size(label, size.width, size.height);
  addStyles(label, styles);
  lv_obj_align(label, position.align, position.x, position.y);
  return label;
}

// Commonly used styles.
lv_style_t Screen::styleButtonIdle;
lv_style_t Screen::styleButtonPressed;
lv_style_t Screen::styleBG;
lv_style_t Screen::leftBorder;
lv_style_t Screen::rightBorder;
lv_style_t Screen::styleTitle;
lv_style_t Screen::styleDropDownIdle;
lv_style_t Screen::styleDropDownPressed;
lv_style_t Screen::styleChart;

// Screens.
lv_obj_t *Screen::loadingScreen;
lv_obj_t *Screen::mainMenuScreen;
lv_obj_t *Screen::homeScreen;
lv_obj_t *Screen::routinesScreen;
lv_obj_t *Screen::logScreen;
lv_obj_t *Screen::graphScreen;
lv_obj_t *Screen::mapScreen;
} // namespace GUI
} // namespace atum