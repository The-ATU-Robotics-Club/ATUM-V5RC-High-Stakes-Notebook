#include "intake.hpp"

namespace atum {
Intake::Intake(std::unique_ptr<Motor> iMtr,
               std::unique_ptr<ColorSensor> iColorSensor,
               Ladybrown *iLadybrown,
               const Parameters &iParams,
               const Logger::Level loggerLevel) :
    Task{this, loggerLevel},
    mtr{std::move(iMtr)},
    colorSensor{std::move(iColorSensor)},
    ladybrown{iLadybrown},
    logger{loggerLevel},
    params{iParams} {
  logger.info("Intake is constructed!");
  stop();
}

void Intake::intake() {
  if(state == IntakeState::Jammed || state == IntakeState::Sorting) {
    return;
  }
  forceIntake(IntakeState::Intaking);
}

void Intake::index() {
  if(state == IntakeState::Jammed || state == IntakeState::Sorting) {
    return;
  }
  forceIntake(IntakeState::Indexing);
}

void Intake::load() {
  if(state == IntakeState::Jammed || state == IntakeState::Sorting ||
     state == IntakeState::FinishedLoading) {
    return;
  }
  forceIntake(IntakeState::Loading);
}

void Intake::outtake() {
  state = IntakeState::Outtaking;
}

void Intake::stop() {
  state = IntakeState::Idle;
}

void Intake::setSortOutColor(const ColorSensor::Color iSortOutColor) {
  sortOutColor = iSortOutColor;
}

ColorSensor::Color Intake::getSortOutColor() const {
  return sortOutColor;
}

int Intake::getCount() {
  return colorSensor->getCount();
}

void Intake::resetCount() {
  colorSensor->resetCount();
}

void Intake::intaking() {
  colorSensor->tallyCount();
  if(ladybrown->mayConflictWithIntake()) {
    params.timerUntilJamChecks.setTime();
    if(state != IntakeState::FinishedLoading) {
      finishLoading();
    }
    return;
  } else if(state == IntakeState::FinishedLoading) {
    state = IntakeState::Loading;
    return;
  }
  if(shouldSort()) {
    state = IntakeState::Sorting;
    return;
  }
  if(shouldIndex()) {
    if(colorSensor->getColor() != ColorSensor::Color::None) {
      params.timerUntilJamChecks.setTime();
      mtr->brake();
      return;
    }
  }
  if(params.timerUntilJamChecks.goneOff() &&
     mtr->getVelocity() < params.jamVelocity) {
    state = IntakeState::Jammed;
  }
  mtr->moveVoltage(12.0);
}

void Intake::unjamming() {
  mtr->moveVoltage(-12);
  wait(params.timeUntilUnjammed);
  if(returnState == IntakeState::Loading) {
    ladybrown->prepare();
  }
  forceIntake(returnState);
}

void Intake::sorting() {
  if(!ladybrown->noRingDetection() &&
     ladybrown->getClosestNamedPosition() == LadybrownState::Loading) {
    ladybrown->prepare();
  }
  mtr->moveVoltage(12);
  Timer timeout{params.generalTimeout};
  while(shouldSort() && !timeout.goneOff()) {
    if(params.timerUntilJamChecks.goneOff() &&
       mtr->getVelocity() < params.jamVelocity) {
      state = IntakeState::Jammed;
      return;
    }
    wait(ColorSensor::refreshRate);
  }
  // Short delay after seems to provide minor advantage.
  wait();
  mtr->moveVoltage(-12);
  wait(params.sortThrowTime);
  forceIntake(returnState);
}

void Intake::finishLoading() {
  mtr->moveVoltage(12);
  wait(params.pressLoadTime);
  mtr->moveVoltage(-12);
  wait(params.finishLoadingTime);
  state = IntakeState::FinishedLoading;
  ladybrown->prepare();
}

void Intake::forceIntake(const IntakeState newState) {
  // It should at least be at a standstill before checks occur. This is to
  // prevent false jams if going from outtaking to intaking quickly.
  if((state != IntakeState::Intaking && state != IntakeState::Indexing &&
      state != IntakeState::Loading) ||
     mtr->getVelocity() < 0_rpm) {
    params.timerUntilJamChecks.setTime();
  }
  state = newState;
  returnState = newState;
}

bool Intake::shouldIndex() const {
  bool ladybrownNotInPosition{state == IntakeState::Loading &&
                              ladybrown->getClosestNamedPosition() !=
                                  LadybrownState::Loading};
  // If indexing or loading while the ladybrown isn't ready, index.
  return colorSensor->check() &&
         (state == IntakeState::Indexing || ladybrownNotInPosition);
}

bool Intake::shouldSort() const {
  return colorSensor->check() && sortOutColor != ColorSensor::Color::None &&
         colorSensor->getColor() == sortOutColor;
}

TASK_DEFINITIONS_FOR(Intake) {
  START_TASK("Intake State Machine")
  while(true) {
    switch(state) {
      case IntakeState::Idle: mtr->brake(); break;
      case IntakeState::Loading:
        if((ladybrown->noRingDetection() &&
            ladybrown->getClosestNamedPosition() == LadybrownState::Resting) ||
           (ladybrown->getClosestNamedPosition() != LadybrownState::Loading &&
            !ladybrown->noRingDetection() && !ladybrown->hasRing())) {
          ladybrown->load();
        }
        // fall through
      case IntakeState::FinishedLoading:
      case IntakeState::Indexing:
      case IntakeState::Intaking: intaking(); break;
      case IntakeState::Outtaking: mtr->moveVoltage(-12); break;
      case IntakeState::Jammed: unjamming(); break;
      case IntakeState::Sorting: sorting(); break;
    }
    wait();
  }
  END_TASK
}
} // namespace atum