#include "tutorial.h"

Tutorial::Tutorial(ClientState *state) :
    Game(state),
    sky({3200, 900}),
    renderSystem(&sky) {
  sky.linkSystem(&renderSystem);
  name = "tutorial";

  plane = sky.joinPlane(0);
  sky.spawnPlane(0, {200, 200}, 0, {});
}

void Tutorial::tick(float delta) {
  if (inFocus) sky.tick(delta); // if this were multiplayer of course
  // we wouldn't have this liberty
}

void Tutorial::render(ui::Frame &f) {
  renderSystem.render(f, plane->state
                         ? plane->state->pos : sf::Vector2f(0, 0));
}

void Tutorial::handle(const sf::Event &event) {
  controller.handle(event);
  if (plane->state) controller.setState(*plane->state);

  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Escape) {
      inFocus = false;
    }
  }
}
