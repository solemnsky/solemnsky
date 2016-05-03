#include "tutorial.h"

Tutorial::Tutorial(ClientShared &state) :
    Game(state, "tutorial"),
    arena(sky::ArenaInitializer("tutorial", "test1")),
    sky(arena, sky::SkyInitializer()),
    skyRender(arena, sky, shared.settings.enableDebug) {
  arena.connectPlayer("offline player");
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  player = arena.getPlayer(0);
  player->spawn({}, {30, 30}, 0);
  status = "some status";
}

/**
 * Game interface.
 */

void Tutorial::onChangeSettings(const SettingsDelta &settings) {
  if (settings.nickname) player->getNickname() = *settings.nickname;
  if (settings.enableDebug) skyRender.enableDebug = settings.enableDebug.get();
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
  skyRender.render(
      f, plane.isSpawned() ?
         plane.getVital()->getState().physical.pos : sf::Vector2f(0, 0));
}

bool Tutorial::handle(const sf::Event &event) {
  if (auto action = shared.triggerSkyAction(event)) {
    player->doAction(action->first, action->second);
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
