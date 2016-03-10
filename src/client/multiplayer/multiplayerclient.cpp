#include "multiplayerclient.h"

/**
 * MultiplayerClientShared.
 */

/**
 * MultiplayerClient.
 */
MultiplayerClient::MultiplayerClient(ClientShared &state,
                                     const std::string &serverHostname,
                                     const unsigned short serverPort) :
    Game(state, "multiplayer"),
    connection(serverHostname, serverPort) { }

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
  if (!shared.server) {
    quitting = true;
  } else {
    shared.host.disconnect(shared.server);
    appLog("Disconnecting from server...", LogOrigin::Client);
    shared.disconnecting = true;
    shared.disconnectTimeout.reset();
  }
}

void MultiplayerClient::tick(float delta) {
  /**
   *
   */

}

void MultiplayerClient::render(ui::Frame &f) {
  if (server && !disconnecting)
    f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});

  chatEntry.render(f);
  messageLog.render(f);

  if (!server) {
    f.drawText({400, 400}, {"Connecting..."}, 60, sf::Color::White);
    return;
  }

  if (disconnecting) {
    f.drawText({400, 400}, {"Disconnecting..."}, 60, sf::Color::White);
    return;
  }

  if (server and arena) {
    switch (arena->mode) {
      case sky::ArenaMode::Lobby: {
        renderLobby(f);
        break;
      }
      case sky::ArenaMode::Game: {
        renderGame(f);
        break;
      }
      case sky::ArenaMode::Scoring: {
        renderScoring(f);
        break;
      }
    }
  }
}

bool MultiplayerClient::handle(const sf::Event &event) {
  if (chatEntry.handle(event)) return true;
  if (readyButton.handle(event)) return true;
  if (event.type == sf::Event::EventType::KeyPressed) {
    if (event.key.code == sf::Keyboard::Return) {
      chatEntry.focus();
      return true;
    }
  }
  return false;
}

void MultiplayerClient::signalRead() {
  if (arena) {
    if (chatEntry.inputSignal)
      transmitServer(
          sky::ClientPacket::Chat(std::string(*chatEntry.inputSignal)));
    if (readyButton.clickSignal)
      transmitServer(sky::ClientPacket::ReqTeamChange(1));
  }
}

void MultiplayerClient::signalClear() {
  chatEntry.signalClear();
  readyButton.signalClear();
}
