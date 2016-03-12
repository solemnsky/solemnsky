#include "homepage.h"
#include "util/methods.h"
#include "client/multiplayer/multiplayer.h"
#include "client/tutorial/tutorial.h"

HomePage::HomePage(ClientShared &clientState) :
    Page(clientState),
    style(),
    tutorialButton(style.tutorialButtonPos, style.tutorialButtonDesc),
    localhostButton(style.localhostButtonPos, style.localhostButtonDesc) { }

void HomePage::onBlur() {
  tutorialButton.reset();
  localhostButton.reset();
}

void HomePage::onFocus() { }

void HomePage::tick(float delta) {
  tutorialButton.tick(delta);
  localhostButton.tick(delta);
}

void HomePage::onChangeSettings(const SettingsDelta &settings) {

}

void HomePage::render(ui::Frame &f) {
  drawBackground(f);

  const float cycleTime =
      shared.appState->profiler.logicTime.average() +
          shared.appState->profiler.renderTime.average();

  const float actualCycleTime = shared.appState->profiler.cycleTime.average();

  tutorialButton.render(f);
  localhostButton.render(f);
}

bool HomePage::handle(const sf::Event &event) {
  if (tutorialButton.handle(event)) return true;
  return localhostButton.handle(event);
}

void HomePage::signalRead() {
  if (tutorialButton.clickSignal)
    shared.beginGame(std::make_unique<Tutorial>(shared));

  if (localhostButton.clickSignal)
    shared.beginGame(
        std::make_unique<Multiplayer>(shared, "localhost", 4242));

}

void HomePage::signalClear() {
  tutorialButton.signalClear();
  localhostButton.signalClear();
}

