#include "gui.hpp"

namespace atum {
static Timer timer{};
static lv_obj_t *autonSelectorScreen;
static lv_obj_t *colorScreen;
static lv_obj_t *skillsScreen;
static lv_obj_t *infoLabel;
static lv_obj_t *colorLabel;
static lv_obj_t *redBtn;
static lv_obj_t *blueBtn;
static lv_obj_t *skillsBtn;
static lv_obj_t *redLabel;
static lv_obj_t *blueLabel;
static lv_obj_t *skillsLabel;
static lv_obj_t *backBtn;
static lv_obj_t *backLabel;
static lv_obj_t *autonList;
static lv_obj_t *backBtn2;
static lv_obj_t *backLabel2;
static lv_obj_t *skillsInfoLabel;
static lv_obj_t *graphScreen;
static lv_obj_t *chart;
static lv_obj_t *title;
static lv_chart_series_t *redSeries;
static lv_chart_series_t *greenSeries;
static lv_chart_series_t *blueSeries;

static lv_obj_t *createBtn(lv_obj_t *parent,
                           int id,
                           lv_coord_t x,
                           lv_coord_t y,
                           lv_coord_t w,
                           lv_coord_t h) {
  lv_obj_t *btn = lv_btn_create(parent, NULL);
  lv_obj_set_free_num(btn, id);
  lv_obj_set_size(btn, w, h);
  lv_obj_set_pos(btn, x, y);
  return btn;
}

static lv_obj_t *createLabel(lv_obj_t *parent,
                             lv_coord_t x,
                             lv_coord_t y,
                             lv_coord_t w,
                             lv_coord_t h,
                             lv_label_long_mode_t lm,
                             const char *title) {
  lv_obj_t *label = lv_label_create(parent, NULL);
  lv_label_set_text(label, title);
  lv_obj_set_x(label, x);
  lv_obj_set_y(label, y);
  lv_label_set_long_mode(label, lm);
  lv_obj_set_width(label, w);
  lv_obj_set_height(label, h);
  return label;
}

void GUI::autonSelector() {
  lvMutex.take();
  lv_scr_load(autonSelectorScreen);
  lvMutex.give();
}

Color GUI::getColor() {
  colorMutex.take();
  const Color currentColor{color};
  colorMutex.give();
  return currentColor;
}

Routine GUI::getRoutine() {
  routineMutex.take();
  const Routine currentRoutine{routine};
  routineMutex.give();
  return currentRoutine;
}

void GUI::graph(const std::string &titleText,
                const std::pair<lv_coord_t, lv_coord_t> &range,
                const std::size_t numOfPoints) {
  lvMutex.take();
  lv_label_set_text(title, titleText.c_str());
  lv_chart_set_point_count(chart, numOfPoints);
  lv_chart_set_range(chart, range.first, range.second);
  lv_scr_load(graphScreen);
  lvMutex.give();
}

void GUI::redPlot(const lv_coord_t value) {
  redMutex.take();
  redQueue.push(value);
  redMutex.give();
}

void GUI::greenPlot(const lv_coord_t value) {
  greenMutex.take();
  greenQueue.push(value);
  greenMutex.give();
}

void GUI::bluePlot(const lv_coord_t value) {
  blueMutex.take();
  blueQueue.push(value);
  blueMutex.give();
}

GUI::GUI() : Task{{"GUI"}} {
  graphScreen = lv_obj_create(NULL, NULL);
  title = lv_label_create(graphScreen, NULL);
  lv_obj_align(title, graphScreen, LV_ALIGN_IN_TOP_MID, 0, 0);
  chart = lv_chart_create(graphScreen, NULL);
  lv_obj_set_size(chart, 400, 200);
  lv_obj_align(chart, graphScreen, LV_ALIGN_CENTER, 0, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  redSeries = lv_chart_add_series(chart, LV_COLOR_RED);
  greenSeries = lv_chart_add_series(chart, LV_COLOR_GREEN);
  blueSeries = lv_chart_add_series(chart, LV_COLOR_BLUE);

  autonSelectorScreen = lv_obj_create(NULL, NULL);
  redBtn = createBtn(autonSelectorScreen, 0, 0, 0, 240, 120);
  blueBtn = createBtn(autonSelectorScreen, 1, 240, 0, 240, 120);
  skillsBtn = createBtn(autonSelectorScreen, 2, 120, 120, 240, 120);
  redLabel = createLabel(redBtn, NULL, NULL, NULL, NULL, NULL, "RED");
  blueLabel = createLabel(blueBtn, NULL, NULL, NULL, NULL, NULL, "BLUE");
  skillsLabel = createLabel(skillsBtn, NULL, NULL, NULL, NULL, NULL, "SKILLS");
  lv_btn_set_action(redBtn, LV_BTN_ACTION_CLICK, pushAction);
  lv_btn_set_action(blueBtn, LV_BTN_ACTION_CLICK, pushAction);
  lv_btn_set_action(skillsBtn, LV_BTN_ACTION_CLICK, pushAction);
  colorScreen = lv_obj_create(NULL, NULL);
  backBtn = createBtn(colorScreen, 3, 320, 160, 120, 60);
  backLabel = createLabel(backBtn, NULL, NULL, NULL, NULL, NULL, "Back");
  infoLabel = createLabel(colorScreen,
                          brainScreenWidth / 2 + 5,
                          60,
                          250,
                          NULL,
                          LV_LABEL_LONG_BREAK,
                          "");
  colorLabel = createLabel(colorScreen, 30, 40, NULL, NULL, NULL, " ");
  lv_obj_align(colorLabel, colorScreen, LV_ALIGN_IN_TOP_LEFT, 40, 10);
  lv_btn_set_action(backBtn, LV_BTN_ACTION_CLICK, pushAction);
  autonList = lv_ddlist_create(colorScreen, NULL);
  std::string routineOptions;
  for(std::size_t i{0}; i < routineName.size() - 2; i++)
    routineOptions += routineName[i] + '\n';
  routineOptions += routineName[routineName.size() - 2];
  lv_ddlist_set_options(autonList, routineOptions.c_str());
  lv_ddlist_set_action(autonList, ddlistAction);
  lv_obj_align(autonList, NULL, LV_ALIGN_IN_TOP_LEFT, 30, 30);
  lv_ddlist_set_hor_fit(autonList, false);
  lv_obj_set_width(autonList, 210);
  skillsScreen = lv_obj_create(NULL, NULL);
  backBtn2 = createBtn(skillsScreen, 4, 320, 160, 120, 60);
  backLabel2 = createLabel(backBtn2, NULL, NULL, NULL, NULL, NULL, "Back");
  skillsInfoLabel = createLabel(skillsScreen,
                                NULL,
                                NULL,
                                400,
                                NULL,
                                LV_LABEL_LONG_BREAK,
                                routineDesc[routineDesc.size() - 1].c_str());
  lv_obj_align(skillsInfoLabel, skillsScreen, LV_ALIGN_CENTER, 0, 0);
  lv_btn_set_action(backBtn2, LV_BTN_ACTION_CLICK, pushAction);

  start();
}
void GUI::taskFn1() {
  while(true) {
    lv_tick_inc(getValueAs<millisecond_t>(timer.timeElapsed()));
    lv_task_handler();
    redMutex.take();
    if(redQueue.size()) {
      lvMutex.take();
      lv_chart_set_next(chart, redSeries, redQueue.front());
      lvMutex.give();
      redQueue.pop();
    }
    redMutex.give();
    greenMutex.take();
    if(greenQueue.size()) {
      lvMutex.take();
      lv_chart_set_next(chart, greenSeries, greenQueue.front());
      lvMutex.give();
      greenQueue.pop();
    }
    greenMutex.give();
    blueMutex.take();
    if(blueQueue.size()) {
      lvMutex.take();
      lv_chart_set_next(chart, blueSeries, blueQueue.front());
      lvMutex.give();
      blueQueue.pop();
    }
    blueMutex.give();
  }
}

lv_res_t GUI::pushAction(lv_obj_t *btn) {
  switch(lv_obj_get_free_num(btn)) {
    case 0:
      color = Color::Red;
      lv_label_set_text(colorLabel, "RED");
      lv_scr_load(colorScreen);
      break;
    case 1:
      color = Color::Blue;
      lv_label_set_text(colorLabel, "BLUE");
      lv_scr_load(colorScreen);
      break;
    case 2:
      color = Color::Red;
      routine = Routine::Skills;
      lv_scr_load(skillsScreen);
      break;
    default: lv_scr_load(autonSelectorScreen); break;
  };
  return LV_RES_OK;
}

lv_res_t GUI::ddlistAction(lv_obj_t *ddlist) {
  lv_label_set_text(infoLabel, "");
  const std::size_t index{lv_ddlist_get_selected(ddlist)};
  routine = static_cast<Routine>(index);
  lv_label_set_text(infoLabel, routineDesc[routine].c_str());
  return LV_RES_OK;
}

GUI GUI::gui{};

Color GUI::color{Color::Red};
Routine GUI::routine{Routine::Quals};
std::queue<int16_t> GUI::redQueue{};
std::queue<int16_t> GUI::greenQueue{};
std::queue<int16_t> GUI::blueQueue{};

pros::Mutex GUI::lvMutex{};
pros::Mutex GUI::colorMutex{};
pros::Mutex GUI::routineMutex{};
pros::Mutex GUI::redMutex{};
pros::Mutex GUI::greenMutex{};
pros::Mutex GUI::blueMutex{};
} // namespace atum