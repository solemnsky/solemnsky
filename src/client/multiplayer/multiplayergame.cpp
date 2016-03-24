#include "multiplayergame.h"
#include "client/elements/style.h"

MultiplayerGame::MultiplayerGame(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Game, shared, connection) {
}

void MultiplayerGame::tick(float delta) { }

void MultiplayerGame::render(ui::Frame &f) { }

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

