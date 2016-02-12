#include "homepage.h"
#include "util/methods.h"
#include "client/game/multiplayerclient.h"

HomePage::HomePage(ClientShared &clientState) :
    Page(clientState),
    tutorialButton({100, 50}, "start tutorial") { }

void HomePage::onLooseFocus() {
  tutorialButton.reset();
}

void HomePage::onFocus() { }

void HomePage::tick(float delta) {
  tutorialButton.tick(delta);
}

void HomePage::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
  const float cycleTime =
      ui::SamplerValue<float>(shared.appState->profiler->logicTime).mean +
      ui::SamplerValue<float>(shared.appState->profiler->renderTime).mean;

  const float actualCycleTime =
      ui::SamplerValue<float>(shared.appState->profiler->cycleTime).mean;

  f.drawText({800, 600},
             {"home page",
              "page where the user receives the comforts of home",
              "uptime: " + std::to_string((int) (shared.uptime * 1000)) + "ms",
              "max FPS: " + std::to_string((int) std::round(1 / cycleTime)),
              "actual FPS: " +
              std::to_string((int) std::round(1 / actualCycleTime))
             },
             40);
  tutorialButton.render(f);
}

bool HomePage::handle(const sf::Event &event) {
  return tutorialButton.handle(event);
}

void HomePage::signalRead() {
  if (tutorialButton.clickSignal)
    shared.beginGame(std::make_unique<MultiplayerClient>(
        shared, "localhost", 4242));
}

void HomePage::signalClear() {
  tutorialButton.signalClear();
}

