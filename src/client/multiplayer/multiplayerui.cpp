#include "multiplayerui.h"
#include "client/elements/style.h"

/**
 * MultiplayerLobby.
 */

MultiplayerLobby::MultiplayerLobby(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Lobby, shared, connection),

    joinButton(style.base.normalButton, style.multi.readyButtonPos, "join"),
    spectateButton(style.base.normalButton, style.multi.readyButtonPos,
                   "spectate"),
    chatInput(style.base.normalTextEntry, style.multi.chatPos, "write here") {
  areChildren({&joinButton, &spectateButton, &chatInput});
}

void MultiplayerLobby::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerLobby::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Lobby), {0, 0}, {0, 0, 1600, 900});

  float offset = 0;
  for (const auto &str : connection.messageLog) {
    offset -= style.base.normalFontSize;
    f.drawText(style.multi.messageLogPos + sf::Vector2f(0, offset),
               str, style.base.normalFontSize);
  }

  ui::Control::render(f);
}

bool MultiplayerLobby::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void MultiplayerLobby::reset() {
  ui::Control::reset();
}

void MultiplayerLobby::signalRead() {
  ui::Control::signalRead();
  if (joinButton.clickSignal)
    connection.transmit(sky::ClientPacket::ReqTeamChange(1));
  if (spectateButton.clickSignal)
    connection.transmit(sky::ClientPacket::ReqTeamChange(1));
}

void MultiplayerLobby::signalClear() {
  ui::Control::signalClear();
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

void MultiplayerScoring::signalRead() {
  ui::Control::signalRead();
}

void MultiplayerScoring::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerScoring::onPacket(const sky::ServerPacket &packet) {

}

void MultiplayerScoring::onChangeSettings(const SettingsDelta &settings) {

}
