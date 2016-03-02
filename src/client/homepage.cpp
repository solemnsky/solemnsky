#include "homepage.h"
#include "util/methods.h"
#include "client/multiplayer/multiplayerclient.h"
#include "client/game/tutorial.h"

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
      ui::SamplerValue<float>(shared.appState->profiler->logicTime).mean +
          ui::SamplerValue<float>(shared.appState->profiler->renderTime).mean;

  const float actualCycleTime =
      ui::SamplerValue<float>(shared.appState->profiler->cycleTime).mean;

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
        std::make_unique<MultiplayerClient>(shared, "localhost", 4242));

}

void HomePage::signalClear() {
  tutorialButton.signalClear();
  localhostButton.signalClear();
}

