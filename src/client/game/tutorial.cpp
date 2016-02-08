#include "tutorial.h"

ui::Button::Style quitButtonStyle() {
  return {};
}

Tutorial::Tutorial(ClientShared &state) :
    Game(state, "tutorial"),
    quitButton({100, 50}, "quit tutorial", quitButtonStyle()),
    sky({}),
    renderSystem(&sky) {
  sky.linkSystem(&renderSystem);

  plane = sky.joinPlane(0);
  sky.spawnPlane(0, {200, 200}, 0, {});
}

void Tutorial::tick(float delta) {
  quitButton.tick(delta);
  if (inFocus)
    sky.tick(delta); // if this were multiplayer of course
  // we wouldn't have this liberty
}

void Tutorial::render(ui::Frame &f) {
  renderSystem.render(f, plane->vital
                         ? plane->vital->pos : sf::Vector2f(0, 0));
  quitButton.render(f);
}

bool Tutorial::handle(const sf::Event &event) {
  if (quitButton.handle(event)) return true;
  if (controller.handle(event)) return true;
  if (plane->vital) controller.setState(*plane->vital);

  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Escape) {
      inFocus = false;
      return true;
    }
  }
  return false;
}

void Tutorial::signalRead() {
  if (quitButton.clickSignal) concluded = true;
}

void Tutorial::signalClear() {
  quitButton.signalClear();
}

