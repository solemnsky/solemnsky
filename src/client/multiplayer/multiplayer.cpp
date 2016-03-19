#include "multiplayer.h"
#include "multiplayerlobby.h"
#include "multiplayergame.h"
#include "multiplayerscoring.h"

/**
 * Multiplayer.
 */

std::unique_ptr<MultiplayerView> Multiplayer::mkView() {
  switch (connection.arena.mode) {
    case sky::ArenaMode::Lobby:
      return std::make_unique<MultiplayerLobby>(shared, connection);
    case sky::ArenaMode::Game:
      return std::make_unique<MultiplayerGame>(shared, connection);
    case sky::ArenaMode::Scoring:
      return std::make_unique<MultiplayerScoring>(shared, connection);
  }
}

Multiplayer::Multiplayer(ClientShared &shared,
                         const std::string &serverHostname,
                         const unsigned short serverPort) :
    Game(shared, "multiplayer"),
    view(nullptr),
    connection(shared, serverHostname, serverPort) { }

/**
 * Game interface.
 */

void Multiplayer::onChangeSettings(const SettingsDelta &settings) {
  if (view) view->onChangeSettings(settings);

  if (connection.myPlayer) {
    if (settings.nickname) {
      sky::PlayerDelta delta(*connection.myPlayer);
      delta.nickname = *settings.nickname;
      connection.transmit(sky::ClientPacket::ReqPlayerDelta(delta));
      // request a nickname change
    }
  }
}

void Multiplayer::onBlur() {

}

void Multiplayer::onFocus() {

}

void Multiplayer::doExit() {
  connection.disconnect();
}

void Multiplayer::tick(float delta) {
  optional<sky::ServerPacket> packet = connection.poll(delta);
  if (connection.disconnected) quitting = true;
  if (connection.disconnecting) return;

  if (connection.connected) {
    if (!view or view->target != connection.arena.mode) view = mkView();
    view->tick(delta);
    if (packet) view->onPacket(*packet);
  }
}

void Multiplayer::render(ui::Frame &f) {
  if (!connection.server) {
    if (connection.disconnecting) {
      auto p = ui::TextProperties::normal;
      p.size = 60;
      f.drawText({400, 400}, {"Disconnecting..."}, p);
      return;
    } else {
      auto p = ui::TextProperties::normal;
      p.size = 60;
      f.drawText({400, 400}, {"Connecting..."}, p);
      return;
    }
  }

  if (view) view->render(f);
}

bool Multiplayer::handle(const sf::Event &event) {
  if (view) return view->handle(event);
  return false;
}

void Multiplayer::reset() {
  if (view) view->reset();
}

void Multiplayer::signalRead() {
  if (view) view->signalRead();
}

void Multiplayer::signalClear() {
  if (view) view->signalClear();
}


