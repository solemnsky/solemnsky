#include "tutorial.h"

Tutorial::Tutorial(ClientShared &state) :
    Game(state, "tutorial"),
    arena(sky::ArenaInitializer("tutorial")),
    sky(arena, sky::SkyInitializer("some map")),
    skyRender(arena, sky) {
  player = &arena.connectPlayer("offline player");
  status = "some status";
}

/**
 * Game interface.
 */

void Tutorial::onChangeSettings(const SettingsDelta &settings) {
  if (settings.nickname) {
    player->nickname = settings.nickname;
  }
}

void Tutorial::onBlur() {

}

void Tutorial::onFocus() {

}

void Tutorial::doExit() {
  quitting = true;
}

/**
 * Control interface.
 */

void Tutorial::tick(float delta) {
  if (shared.ui.gameFocused()) arena.tick(delta);
  // if this were multiplayer of course we wouldn't have this liberty
}

void Tutorial::render(ui::Frame &f) {
  const sky::Plane &plane = sky.getPlane(*player);
  skyRender.render(f, plane.vital ? plane.vital->state.pos : {0, 0})
}

bool Tutorial::handle(const sf::Event &event) {
  if (controller.handle(event)) {
    player->doAction(sky::Action(sky::Action::Type::Left, true));
    // etc
    return true;
  }
  return false;
}

void Tutorial::reset() {
  ui::Control::reset();
}

void Tutorial::signalRead() {
  ui::Control::signalRead();
}

void Tutorial::signalClear() {
  ui::Control::signalClear();
}
