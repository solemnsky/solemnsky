#include "tutorial.h"

Tutorial::Tutorial(ClientState *state) :
    Game(state),
    sky({1600, 900}),
    renderSystem(&sky) {
  sky.linkSystem(&renderSystem);
  name = "tutorial";
}

void Tutorial::tick(float delta) {
  sky.tick(delta);
}

void Tutorial::render(ui::Frame &f) {
  f.drawText({800, 450},
             {"tutorial", "demonstration of play mechanics go here"}, 40);
}

void Tutorial::handle(const sf::Event &event) {
  controller.handle(event);
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Escape) {
      inFocus = false;
    }
    if (event.key.code == sf::Keyboard::F) {
      concluded = true;
    }
  }
}
