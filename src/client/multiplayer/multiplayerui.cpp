#include "multiplayerui.h"

/**
 * MultiplayerLobby.
 */

MultiplayerLobby::MultiplayerLobby(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Lobby, shared, connection) { }

void MultiplayerLobby::tick(float delta) {

}

void MultiplayerLobby::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Lobby), {0, 0}, {0, 0, 1600, 900});
}

bool MultiplayerLobby::handle(const sf::Event &event) {
  return false;
}

void MultiplayerLobby::onBlur() {

}

void MultiplayerLobby::onFocus() {

}

void MultiplayerLobby::onPacket(const sky::ServerPacket &packet) {

}

void MultiplayerLobby::onChangeSettings(const SettingsDelta &settings) {

}

/**
 * MultiplayerGame.
 */

MultiplayerGame::MultiplayerGame(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Game, shared, connection),
    renderSystem(connection.arena.sky.get_ptr()) { }

void MultiplayerGame::tick(float delta) {

}

void MultiplayerGame::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
}

bool MultiplayerGame::handle(const sf::Event &event) {
  return false;
}


void MultiplayerGame::onBlur() {

}

void MultiplayerGame::onFocus() {

}

void MultiplayerGame::onPacket(const sky::ServerPacket &packet) {

}

void MultiplayerGame::onChangeSettings(const SettingsDelta &settings) {

}

/**
 * MultiplayerScoring.
 */

MultiplayerScoring::MultiplayerScoring(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Scoring, shared, connection) { }

void MultiplayerScoring::tick(float delta) {

}

void MultiplayerScoring::render(ui::Frame &f) {

}

bool MultiplayerScoring::handle(const sf::Event &event) {
  return false;
}

void MultiplayerScoring::onBlur() {

}

void MultiplayerScoring::onFocus() {

}

void MultiplayerScoring::onPacket(const sky::ServerPacket &packet) {

}

void MultiplayerScoring::onChangeSettings(const SettingsDelta &settings) {

}
