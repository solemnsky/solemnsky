#include "multiplayerclient.h"

#include "boost/iterator/function_input_iterator.hpp"

MultiplayerClient::MultiplayerClient(ClientShared &state,
                                     const std::string &serverHostname,
                                     const unsigned short serverPort) :
    Game(state, "multiplayer"),

    chatEntry({20, 850}, "[enter] to chat"),
    messageLog({20, 840}),

    host(tg::HostType::Client),
    server(nullptr),
    telegraph(sky::ServerPacketPack(), sky::ClientPacketPack()),
    pingCooldown(5),
    triedConnection(false),
    disconnecting(false),
    disconnectTimeout(1) {
  host.connect(serverHostname, serverPort);
  messageLog.expanded = true;
}

/**
 * Graphics.
 */

void MultiplayerClient::setUI(const sky::ArenaMode mode) {
  switch (mode) {
    case sky::ArenaMode::Lobby: {
      messageLog.expanded = true;
      messageLog.style.maxWidth = style.lobbyChatWidth;
    }
    case sky::ArenaMode::Game:
      break;
    case sky::ArenaMode::Scoring:
      break;
  }
}

void MultiplayerClient::renderLobby(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Lobby), {0, 0}, {0, 0, 1600, 900});
  std::vector<std::string> players;
  for (sky::Player &player : arena->players)
    players.push_back(player.nickname);
  f.drawText({style.lobbyPlayersOffset, style.lobbyTopMargin},
             players.begin(), players.end(), style.lobbyFontSize);
  // TODO: don't copy strings
}

void MultiplayerClient::renderGame(ui::Frame &f) {
  if (!(arena and arena->sky))
    return;
  // trying to render without being in the arena, having a sky, and identifying
  // with a player

  if (sky::Plane *plane = arena->sky->getPlane(myPlayer->pid)) {
    renderSystem->render(f, plane->state.pos);
  } else {
    renderSystem->render(f, {0, 0}); // TODO: panning in spectator mode
  }
}

void MultiplayerClient::renderScoring(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Lobby), {0, 0}, {0, 0, 1600, 900});
}

void MultiplayerClient::renderScoreOverlay(ui::Frame &f) {
  const static sf::Vector2f scoreOverlayPos = 0.5f *
      sf::Vector2f(
          1600.0f - style.scoreOverlayDims.x,
          900.0f - style.scoreOverlayDims.y);
  f.drawSprite(
      textureOf(Res::ScoreOverlay), scoreOverlayPos,
      {0, 0, style.scoreOverlayDims.x, style.scoreOverlayDims.y});
}

/**
 * Networking submethods.
 */

void MultiplayerClient::transmitServer(const sky::ClientPacket &packet) {
  if (server) telegraph.transmit(host, server, packet);
}

bool MultiplayerClient::processPacket(const sky::ServerPacket &packet) {
  // received a packet from the server
  using namespace sky;

  if (!arena) {
    // waiting for the arena connection request to be accepted
    if (packet.type == ServerPacket::Type::AckJoin) {
      if (!(packet.arenaInitializer and packet.pid))
        return false;

      arena.emplace();
      if (!arena->applyInitializer(*packet.arenaInitializer)) {
        arena.reset();
        return false;
      }

      myPlayer = arena->getPlayer(*packet.pid);
      setUI(arena->mode);
      if (arena->mode == sky::ArenaMode::Game) {
        renderSystem.emplace(&*arena->sky);
      }

      appLog("Joined arena!", LogOrigin::Client);
      return true; // server sent us an AckJoin when we're not registered
    }
    return false; // we're only interested in AckJoins until we're connected
  }

  // we're in the arena
  switch (packet.type) {
    case ServerPacket::Type::Pong:
      return true; // received a pong from the server

    case ServerPacket::Type::Message: {
      if (!packet.message) return false;
      switch (packet.message->type) {
        case ServerMessage::Type::Chat: {
          if (!packet.message->from) return false;
          messageLog.pushEntry(*packet.message->from +
              ": " + packet.message->contents);
          break;
        }
        case ServerMessage::Type::Broadcast: {
          messageLog.pushEntry("[server]: " + packet.message->contents);
          break;
        }
      }
      return true; // server sent us a Message
    }

    case ServerPacket::Type::NoteArenaDelta: {
      if (!packet.arenaDelta) return false;
      if (!arena->applyDelta(*packet.arenaDelta)) return false;

      if (packet.arenaDelta->type == sky::ArenaDelta::Type::Mode) {
        // things can happen when the mode changes...
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
}

void MultiplayerClient::onFocus() {

}

void MultiplayerClient::onChangeSettings(const SettingsDelta &settings) {
  if (settings.nickname) {
    sky::PlayerDelta delta(*settings.nickname);
    transmitServer(sky::ClientPacket::ReqDelta(delta));
    // request a nickname change
  }
}

void MultiplayerClient::doExit() {
  if (!server) {
    quitting = true;
  } else {
    host.disconnect(server);
    appLog("Disconnecting from server...", LogOrigin::Client);
    disconnecting = true;
    disconnectTimeout.reset();
  }
}

/**
 * Control interface.
 */

void MultiplayerClient::tick(float delta) {
  chatEntry.tick(delta);
  messageLog.tick(delta);

  using namespace sky;

  /**
   * Here we handle connections and disconnections from the server, and send
   * the ReqJoin when we manage to make initial contact. All the rest of the
   * protocol is implemented in processPack().
   */

  event = host.poll();
  if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
    server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    quitting = true;
    return;
  }

  if (disconnecting) {
    if (disconnectTimeout.cool(delta)) {
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
  if (event.type == sf::Event::EventType::KeyPressed) {
    if (event.key.code == sf::Keyboard::Return) {
      chatEntry.focus();
      return true;
    }
  }
  return false;
}

void MultiplayerClient::signalRead() {
  if (chatEntry.inputSignal) {
    if (arena)
      transmitServer(
          sky::ClientPacket::Chat(std::string(*chatEntry.inputSignal)));
  }
}

void MultiplayerClient::signalClear() {
  chatEntry.signalClear();
}
