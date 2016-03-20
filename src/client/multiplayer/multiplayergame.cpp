#include "multiplayergame.h"
#include "client/elements/style.h"

MultiplayerGame::MultiplayerGame(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Game, shared, connection),
    sky(*connection.arena.sky),
    renderSystem(&sky),
    skyDeltaUpdate(0.1) {
  sky.linkSystem(&renderSystem);
}

void MultiplayerGame::tick(float delta) {
  if (skyDeltaUpdate.cool(delta)) {
    skyDeltaUpdate.reset();
    connection.transmit(sky::ClientPacket::ReqSkyDelta(
        sky.collectDelta()
    ));
  }
}

void MultiplayerGame::render(ui::Frame &f) {
  if (sky::Plane *plane =
      sky.getPlane(connection.myPlayer->pid)) {
    renderSystem.render(f, plane->state.pos);
  } else renderSystem.render(f, {0, 0});

  f.drawText({300, 300}, "need to implement this", sf::Color::White,
             style.base.normalText);
}

bool MultiplayerGame::handle(const sf::Event &event) {
  return false;
}

void MultiplayerGame::signalRead() {
  ui::Control::signalRead();
}

void MultiplayerGame::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerGame::onPacket(const sky::ServerPacket &packet) {

}

void MultiplayerGame::onChangeSettings(const SettingsDelta &settings) {

}

