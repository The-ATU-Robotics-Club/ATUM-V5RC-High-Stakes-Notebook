#include "log.hpp"

namespace atum {
namespace GUI {
const std::size_t Log::maxLogLines{100};

void Log::write(const std::string &msg) {
  std::string logText{lv_label_get_text(logTextLabel)};
  if(logLines >= maxLogLines) {
    logText.erase(0, logText.find('\n') + 1);
  } else {
    logLines++;
  }
  logText += msg + '\n';
  lv_label_set_text(logTextLabel, logText.c_str());
}

void Log::setupScreen() {
  createLabel(logScreen,
              "LOG",
              {250, defaultHeight},
              {defaultPadding, defaultPadding},
              {&rightBorder, &styleTitle});

  createScreenChangeButton(
      logScreen,
      LV_SYMBOL_NEW_LINE,
      {150, defaultHeight},
      {-defaultPadding, defaultPadding, LV_ALIGN_TOP_RIGHT},
      mainMenuScreen,
      {&leftBorder});

  lv_obj_t *logTextBackground = lv_obj_create(logScreen);
  lv_obj_align(logTextBackground, LV_ALIGN_TOP_MID, 0, contentYOffset);
  lv_obj_set_size(logTextBackground, fillWidth, fillHeight);
  lv_obj_set_style_bg_color(logTextBackground, lightGrey, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(logTextBackground, LV_OPA_COVER, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(
      logTextBackground, white, LV_STATE_DEFAULT | LV_PART_SCROLLBAR);
  lv_obj_set_scrollbar_mode(logTextBackground, LV_SCROLLBAR_MODE_ACTIVE);
  lv_obj_set_scroll_dir(logTextBackground, LV_DIR_VER);

  logTextLabel = lv_label_create(logTextBackground);
  lv_label_set_text(logTextLabel, ""); // Clear default text.
  lv_obj_align(logTextLabel, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_text_color(logTextLabel, black, LV_STATE_DEFAULT);
}

lv_obj_t *Log::logTextLabel;
std::string Log::logText;
std::size_t Log::logLines;
} // namespace GUI
} // namespace atum