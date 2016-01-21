#include "tutorial.h"

ui::Button::Style quitButtonStyle() {
  return {};
}

Tutorial::Tutorial(ClientState *state) :
    quitButton({100, 50}, "quit tutorial", quitButtonStyle()),
    Game(state),
    sky({3200, 900}),
    renderSystem(&sky) {
  sky.linkSystem(&renderSystem);
  name = "tutorial";

  plane = sky.joinPlane(0);
  sky.spawnPlane(0, {200, 200}, 0, {});
}

void Tutorial::tick(float delta) {
  quitButton.tick(delta);
  if (inFocus) sky.tick(delta); // if this were multiplayer of course
  // we wouldn't have this liberty
}

void Tutorial::render(ui::Frame &f) {
  renderSystem.render(f, plane->state
                         ? plane->state->pos : sf::Vector2f(0, 0));
  quitButton.render(f);
}

void Tutorial::handle(const sf::Event &event) {
  quitButton.handle(event);
  controller.handle(event);
  if (plane->state) controller.setState(*plane->state);

  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Escape) {
      inFocus = false;
    }
  }
}

void Tutorial::signalRead() {
  if (!quitButton.clickSignal.empty()) { concluded = true; }

}

void Tutorial::signalClear() {
  quitButton.signalClear();
}

