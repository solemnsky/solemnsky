#include "tutorial.h"

Tutorial::Tutorial(ClientShared &state) :
    Game(state, "tutorial"),
    sky("default_map_that_exists"),
    renderSystem(&sky) {
  sky.linkSystem(&renderSystem);

  plane = &sky.addPlane(0, {}, {200, 200}, 0);
}

/**
 * Game interface.
 */

void Tutorial::onBlur() {

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
  renderSystem.render(f, plane ? plane->state.pos : sf::Vector2f(0, 0));
}

bool Tutorial::handle(const sf::Event &event) {
  if (controller.handle(event)) {
    if (plane) controller.setState(plane->state);
    return true;
  }
  return false;
}

void Tutorial::signalRead() { }

void Tutorial::signalClear() { }
