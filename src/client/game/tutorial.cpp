#include "tutorial.h"

Tutorial::Tutorial(ClientShared &state) :
    Game(state, "tutorial"),
    sky({}),
    renderSystem(&sky) {
  sky.linkSystem(&renderSystem);

  plane = &sky.joinPlane(0);
  sky.spawnPlane(0, {200, 200}, 0, {});
}

/**
 * Game interface.
 */

void Tutorial::onLooseFocus() {

}

void Tutorial::onFocus() {

}

void Tutorial::onChangeSettings(const SettingsDelta &settings) {
  if (settings.nickname) {
    // a simple Sky doesn't hold nickname data!
  }
}

void Tutorial::doExit() {
  quitting = true;
}

/**
 * Control interface.
 */

void Tutorial::tick(float delta) {
  if (shared.ui.gameFocused())
    sky.tick(delta); // if this were multiplayer of course
  // we wouldn't have this liberty
}

void Tutorial::render(ui::Frame &f) {
  renderSystem.render(f, plane->vital
                         ? plane->vital->pos : sf::Vector2f(0, 0));
}

bool Tutorial::handle(const sf::Event &event) {
  if (controller.handle(event)) {
    if (plane->vital) controller.setState(*plane->vital);
    return true;
  }

  if (event.type == sf::Event::KeyPressed
      && event.key.code == sf::Keyboard::Escape) {
    shared.unfocusGame();
    return true;
  }

  return false;
}

void Tutorial::signalRead() { }

void Tutorial::signalClear() { }
