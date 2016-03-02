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

    mShared(serverHostname, serverPort),
    chatEntry(style.chatPos, "[enter] to chat"),
    messageLog(style.messageLogPos),
    readyButton(style.readyButtonPos, "ready") {
  messageLog.expanded = true;
}

void MultiplayerClient::renderScoreOverlay(ui::Frame &f) {
}

/**
 * Networking submethods.
 */

bool MultiplayerClient::processPacket(const sky::ServerPacket &packet) {
  /**
   * Here we deal with server protocol packets.
   */
  using namespace sky;

  if (!mShared.arena) {
    // waiting for the arena connection request to be accepted
    if (packet.type == ServerPacket::Type::AckJoin) {
      if (!(packet.arenaInitializer and packet.pid))
        return false;

      shared.arena.emplace();
      if (!arena->applyInitializer(*packet.arenaInitializer)) {
        arena.reset();
        return false; // initializer didn't work
      }

      myPlayer = arena->getPlayer(*packet.pid);
      if (!myPlayer) {
        arena.reset();
        return false; // pid invalid
      }

      setUI(arena->mode);
      if (arena->mode == sky::ArenaMode::Game) {
        renderSystem.emplace(&*arena->sky);
      }

      appLog("Joined arena!", LogOrigin::Client);
      return true;
    }
    return false; // we're only interested in AckJoins until we're connected
  }

  // we're in the arena
  switch (packet.type) {
    case ServerPacket::Type::Pong: {
      return true; // received a pong from the server
    }

    case ServerPacket::Type::Message: {
      if (!packet.message) return false;
      switch (packet.message->type) {
        case ServerMessage::Type::Chat: {
          if (!packet.message->from) return false;
          messageLog.pushEntry("[chat]" + *packet.message->from +
              ": " + packet.message->contents);
          break;
        }
        case ServerMessage::Type::Broadcast: {
          messageLog.pushEntry(packet.message->contents);
          break;
        }
      }
      return true; // server sent us a Message
    }

    case ServerPacket::Type::NoteArenaDelta: {
      if (!packet.arenaDelta) return false;
      if (!arena->applyDelta(*packet.arenaDelta)) return false;

      if (packet.arenaDelta->type == sky::ArenaDelta::Type::Mode) {
        // we want to listen for changes in the mode
        setUI(arena->mode);
        if (arena->mode == sky::ArenaMode::Game)
          renderSystem.emplace(&*arena->sky);
      }

      return true; // server sent us a NoteArenaDelta
    }

    case ServerPacket::Type::NoteSkyDelta: {
      if (!packet.skyDelta) return false;
      if (arena->sky) arena->sky->applyDelta(*packet.skyDelta);
      return true; // server sent us a NoteSkyDelta
    }

    default:
      break;
  }

  return false; // invalid packet type, considering we're in the arena
}

/**
 * Game interface.
 */

void MultiplayerClient::onBlur() {
  chatEntry.reset();
  readyButton.reset();
}

void MultiplayerClient::onFocus() {

}

void MultiplayerClient::onChangeSettings(const SettingsDelta &settings) {
  if (settings.nickname) {
    sky::PlayerDelta delta(*shared.myPlayer);
    delta.nickname = *settings.nickname;
    shared.transmitServer(sky::ClientPacket::ReqDelta(delta));
    // request a nickname change
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

  netEvent = shared.host.poll();
  if (netEvent.type == ENET_EVENT_TYPE_DISCONNECT) {
    shared.server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    quitting = true;
    return;
  }

  if (shared.disconnecting) {
    if (shared.disconnectTimeout.cool(delta)) {
      appLog("Disconnecting from unresponsive server!", LogOrigin::Client);
      quitting = true;
    }
    return;
  }

  if (!server) {
    // still trying to connect to the server...
    if (event.type == ENET_EVENT_TYPE_CONNECT) {
      server = event.peer;
      appLog("Connected to server!", LogOrigin::Client);
    }
  } else {
    // connected
    if (!triedConnection) {
      // we have a link but haven't sent an arena connection request
      appLog("Asking to join arena...", LogOrigin::Client);
      transmitServer(ClientPacket::ReqJoin(shared.settings.nickname));
      triedConnection = true;
      return;
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      if (const auto reception =
          telegraph.receive(event.packet)) if (processPacket(*reception)) return;
      appLog("Received malformed packet from server!", LogOrigin::Client);
    }
  }
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
