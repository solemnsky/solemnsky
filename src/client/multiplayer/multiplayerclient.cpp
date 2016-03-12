#include "multiplayerclient.h"

/**
 * MultiplayerClient.
 */

void MultiplayerClient::switchView() {
  switch (connection.arena.mode) {
    case sky::ArenaMode::Lobby: {
      view = std::make_unique<MultiplayerLobby>(shared);
      break;
    }
    case sky::ArenaMode::Game: {
      view = std::make_unique<MultiplayerGame>(shared);
      break;
    }
    case sky::ArenaMode::Scoring: {
      view = std::make_unique<MultiplayerScoring>(shared);
      break;
    }
  }
}

MultiplayerClient::MultiplayerClient(ClientShared &state,
                                     const std::string &serverHostname,
                                     const unsigned short serverPort) :
    Game(state, "multiplayer"),
    connection(<#initializer#>, serverHostname, serverPort) { }

/**
 * Game interface.
 */

void MultiplayerClient::onBlur() {
  view->onBlur();
}

void MultiplayerClient::onFocus() {
  view->onFocus();
}

void MultiplayerClient::onChangeSettings(const SettingsDelta &settings) {
  view->onChangeSettings(settings);

  if (connection.myPlayer) {
    if (settings.nickname) {
      sky::PlayerDelta delta(*connection.myPlayer);
      delta.nickname = *settings.nickname;
      connection.transmit(sky::ClientPacket::ReqDelta(delta));
      // request a nickname change
    }
  }
}

void MultiplayerClient::doExit() {
  if (connection.disconnected) {
    appLog("Disconnected!", LogOrigin::Client);
    quitting = true;
  } else {
    connection.disconnect();
    appLog("Disconnecting from server...", LogOrigin::Client);
  }
}

void MultiplayerClient::tick(float delta) {
  optional<sky::ServerPacket> packet = connection.poll(delta);

  // handle a potential mode change
  if (packet) {
    if (packet->type == sky::ServerPacket::Type::NoteArenaDelta) {
      if (packet->arenaDelta->type == sky::ArenaDelta::Type::Mode) {
        switchView();
      }
    }
  }

  if (view) {
    view->tick(delta);
    if (packet) view->onPacket(*packet);
  }
}

void MultiplayerClient::render(ui::Frame &f) {
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

bool MultiplayerClient::handle(const sf::Event &event) {
  if (view) return view->handle(event);
  return false;
}

void MultiplayerClient::signalRead() {
  if (view) view->signalRead();
  if (arena) {
    if (chatEntry.inputSignal)
      transmitServer(
          sky::ClientPacket::Chat(std::string(*chatEntry.inputSignal)));
    if (readyButton.clickSignal)
      transmitServer(sky::ClientPacket::ReqTeamChange(1));
  }
}

void MultiplayerClient::signalClear() {
  if (view) view->signalClear();
}


