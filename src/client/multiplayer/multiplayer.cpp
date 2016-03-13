#include "multiplayer.h"

/**
 * Multiplayer.
 */

void Multiplayer::updateView() {
  switch (connection.arena.mode) {
    case sky::ArenaMode::Lobby: {
      view = std::make_unique<MultiplayerLobby>(shared, connection);
      break;
    }
    case sky::ArenaMode::Game: {
      view = std::make_unique<MultiplayerGame>(shared, connection);
      break;
    }
    case sky::ArenaMode::Scoring: {
      view = std::make_unique<MultiplayerScoring>(shared, connection);
      break;
    }
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

void Multiplayer::onBlur() {
  if (view) view->onBlur();
}

void Multiplayer::onFocus() {
  if (view) view->onFocus();
}

void Multiplayer::onChangeSettings(const SettingsDelta &settings) {
  if (view) view->onChangeSettings(settings);

  if (connection.myPlayer) {
    if (settings.nickname) {
      sky::PlayerDelta delta(*connection.myPlayer);
      delta.nickname = *settings.nickname;
      connection.transmit(sky::ClientPacket::ReqDelta(delta));
      // request a nickname change
    }
  }
}

void Multiplayer::doExit() {
  connection.disconnect();
}

void Multiplayer::tick(float delta) {
  optional<sky::ServerPacket> packet = connection.poll(delta);
  if (connection.disconnecting) {
    if (connection.disconnected) quitting = true;
    return;
  }

  if (connection.connected) {
    if (!view or view->target != connection.arena.mode) updateView();
    view->tick(delta);
    if (packet) view->onPacket(*packet);
  }
}

void Multiplayer::render(ui::Frame &f) {
  if (!connection.server) {
    if (connection.disconnecting) {
      f.drawText({400, 400}, {"Disconnecting..."}, 60, sf::Color::White);
      return;
    } else {
      f.drawText({400, 400}, {"Connecting..."}, 60, sf::Color::White);
      return;
    }
  }

  if (view) view->render(f);
}

bool Multiplayer::handle(const sf::Event &event) {
  if (view) return view->handle(event);
  return false;
}

void Multiplayer::signalRead() {
  if (view) view->signalRead();
}

void Multiplayer::signalClear() {
  if (view) view->signalClear();
}


