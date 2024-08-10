#pragma once

#include "../time/task.hpp"
#include "../time/timer.hpp"
#include "pros/apix.h"
#include <queue>

namespace atum {
enum class Color { Red = 0, Blue };

enum Routine { Quals = 0, Elims, WP, Nothing, Skills };

const std::array<std::string, Routine::Skills + 1> routineName{"Quals",
                                                               "Elims",
                                                               "WP",
                                                               "Nothing",
                                                               "Skills"};

const std::array<std::string, Routine::Skills + 1> routineDesc{
    "Routine for normal quals matches; should get WP.",
    "Routine for elims; no WP.",
    "Safer route for WP.",
    "Does nothing.",
    "Skills configuration."};

class GUI : public Task {
  public:
  GUI(GUI const &) = delete;
  GUI &operator=(GUI const &) = delete;

  static void autonSelector();

  static Color getColor();

  static Routine getRoutine();

  static void graph(const std::string &titleText,
                    const std::pair<lv_coord_t, lv_coord_t> &range =
                        std::make_pair(-100, 100),
                    const std::size_t numOfPoints = 100);

  static void redPlot(const lv_coord_t value);

  static void greenPlot(const lv_coord_t value);

  static void bluePlot(const lv_coord_t value);

  private:
  GUI();

  void taskFn1() override;

  static lv_res_t pushAction(lv_obj_t *btn);

  static lv_res_t ddlistAction(lv_obj_t *ddlist);

  static void setPoints(const std::size_t &iPoints);

  static GUI gui;

  static Color color;
  static Routine routine;
  static std::queue<int16_t> redQueue;
  static std::queue<int16_t> greenQueue;
  static std::queue<int16_t> blueQueue;

  static pros::Mutex lvMutex;
  static pros::Mutex colorMutex;
  static pros::Mutex routineMutex;
  static pros::Mutex redMutex;
  static pros::Mutex greenMutex;
  static pros::Mutex blueMutex;
};
} // namespace atum