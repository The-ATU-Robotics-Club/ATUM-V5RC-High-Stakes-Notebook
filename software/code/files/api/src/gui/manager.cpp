#include "manager.hpp"

namespace atum {
namespace GUI {
void Manager::initialize() {
  // Initialization.
  Screen::createScreens();
  Screen::initializeStyles();

  // Screen setup. Can't do routine screen yet.
  loadingScreenSetup();
  homeScreenSetup();
  mainMenuScreenSetup();
  Log::setupScreen();
  Graph::setupScreen();
  Map::setupScreen();
}

void Manager::startLoading(const std::string &routines) {
  // Go to the loading screen until given signal robot is good to go.
  Routines::setupScreen(routines);
  lv_scr_load(loadingScreen);
}

void Manager::finishLoading() {
  lv_scr_load(homeScreen);
}

void Manager::error() {
  // Don't interrupt easter egg.
  if(lv_img_get_src(splashImage) != &kelly) {
    lv_img_set_src(splashImage, &atumerror);
  }
}

void Manager::easteregg() {
  lv_img_set_src(splashImage, &kelly); // Easter egg Randy Kelly!
}

void Manager::loadingScreenSetup() {
  lv_obj_t *loadingLabel{lv_label_create(loadingScreen)};
  lv_label_set_text(loadingLabel, "LOADING!   DO NOT TOUCH!");
  lv_obj_center(loadingLabel);
  lv_obj_set_width(loadingLabel, screenWidth);
  lv_label_set_long_mode(loadingLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_style_text_color(loadingLabel, white, LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(
      loadingLabel, &lv_font_montserrat_48, LV_STATE_DEFAULT);
}

void Manager::homeScreenSetup() {
  splashImage = lv_img_create(homeScreen);
  lv_img_set_src(splashImage, &atumlogo);

  lv_obj_set_style_opa(splashImage, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_center(splashImage);
  lv_obj_align(splashImage, LV_ALIGN_CENTER, 0, 0);

  lv_obj_set_size(splashImage, 427, 240);

  lv_obj_t *clickAnywhereLabel{lv_label_create(homeScreen)};
  lv_label_set_text(clickAnywhereLabel, "Click anywhere to continue.");
  lv_obj_center(clickAnywhereLabel);
  lv_obj_align(clickAnywhereLabel, LV_ALIGN_BOTTOM_MID, 0, -2 * defaultPadding);
  lv_obj_set_style_text_color(clickAnywhereLabel, white, LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(
      clickAnywhereLabel, &lv_font_montserrat_12, LV_STATE_DEFAULT);
}

void Manager::mainMenuScreenSetup() {
  createLabel(mainMenuScreen,
              "MAIN MENU",
              {250, defaultHeight},
              {defaultPadding, defaultPadding},
              {&rightBorder, &styleTitle});

  createScreenChangeButton(
      mainMenuScreen,
      LV_SYMBOL_NEW_LINE,
      {150, defaultHeight},
      {-defaultPadding, defaultPadding, LV_ALIGN_TOP_RIGHT},
      homeScreen,
      {&leftBorder});

  createScreenChangeButton(mainMenuScreen,
                           "ROUTINE",
                           {fillWidth, defaultHeight},
                           {0, contentYOffset, LV_ALIGN_TOP_MID},
                           routinesScreen);

  createScreenChangeButton(
      mainMenuScreen,
      "LOG",
      {fillWidth, defaultHeight},
      {0, contentYOffset + defaultHeight + defaultPadding, LV_ALIGN_TOP_MID},
      logScreen);

  createScreenChangeButton(
      mainMenuScreen,
      "GRAPH",
      {fillWidth, defaultHeight},
      {0,
       contentYOffset + 2 * (defaultHeight + defaultPadding),
       LV_ALIGN_TOP_MID},
      graphScreen);

  createScreenChangeButton(
      mainMenuScreen,
      "MAP",
      {fillWidth, defaultHeight},
      {0,
       contentYOffset + 3 * (defaultHeight + defaultPadding),
       LV_ALIGN_TOP_MID},
      mapScreen);
}

lv_obj_t *Manager::splashImage;
} // namespace GUI
} // namespace atum