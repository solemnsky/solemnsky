#include "tutorial.h"

Tutorial::Tutorial(ClientShared &state) :
    Game(state, "tutorial"),
    arena(sky::ArenaInit("tutorial", "test1", sky::ArenaMode::Game)),
    skyManager(arena, sky::SkyInitializer()),
    skyRender(arena, skyManager, *skyManager.getSky()) {
  arena.connectPlayer("offline player");
  player = arena.getPlayer(0);
  player->spawn({}, {30, 30}, 0);
  participation = &skyManager.getParticipation(*player).get();
  status = "learning to play";
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
  auto &plane = skyManager.getParticipation(*player)->getPlane();
  skyRender.render(
      f, plane ?
         plane->getState().physical.pos :
         sf::Vector2f(0, 0));
}

bool Tutorial::handle(const sf::Event &event) {
  if (auto action = shared.triggerClientAction(event)) {
    if (action->first == ClientAction::Spawn 
        and action->second 
        and !participation->isSpawned()) {
      player->spawn({}, {300, 300}, 0);
      return true;
    }
  }

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
