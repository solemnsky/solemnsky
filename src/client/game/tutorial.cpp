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

}

/**
 * Control interface.
 */

void Tutorial::tick(float delta) {
  quitButton.tick(delta);
  if (shared.ui.gameFocused())
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

void Tutorial::signalRead() {
  if (quitButton.clickSignal) quitting = true;
}

void Tutorial::signalClear() {
  quitButton.signalClear();
}
