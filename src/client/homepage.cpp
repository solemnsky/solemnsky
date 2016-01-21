#include "client/game/tutorial.h"
#include "homepage.h"
#include "base/resources.h"

HomePage::HomePage(ClientState *state) :
    state(state),
    tutorialButton({100, 50}, "start tutorial") { }

void HomePage::tick(float delta) {
  tutorialButton.tick(delta);
}

void HomePage::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
  const float cycleTime =
      ui::SamplerValue<float>(state->appState->profiler->cycleTime).mean;
  f.drawText({800, 600},
             {"home page",
              "page where the user receives the comforts of home",
              "uptime: " + std::to_string((int) (state->uptime * 1000)) + "ms",
              "FPS: " + std::to_string(std::round(1 / cycleTime))
             },
             40);
  tutorialButton.render(f);
}

void HomePage::handle(const sf::Event &event) {
  tutorialButton.handle(event);
}

void HomePage::signalRead() {
  if (!tutorialButton.clickSignal.empty()) {
    state->game = std::make_unique<Tutorial>(state);
  }
}

void HomePage::signalClear() {
  tutorialButton.signalClear();
}
