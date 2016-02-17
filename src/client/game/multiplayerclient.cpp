#include "multiplayerclient.h"
#include "util/methods.h"

MultiplayerClient::MultiplayerClient(ClientShared &state,
                                     const std::string &serverHostname,
                                     const unsigned short serverPort) :
    Game(state, "multiplayer"),

    chatEntry({20, 850}, "[enter] to chat"),
    messageLog({20, 840}),

    host(tg::HostType::Client),
    server(nullptr),
    telegraph(sky::serverPacketPack, sky::clientPacketPack),
    pingCooldown(5),
    triedConnection(false),
    disconnecting(false),
    disconnectTimeout(1) {
  host.connect(serverHostname, serverPort);
  messageLog.expanded = true;
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
      if (!all(packet.arenaInitializer, packet.pid)) return false;

      arena.emplace(*packet.arenaInitializer);
      myRecord = arena->getRecord(*packet.pid);
      appLog("Joined arena!", LogOrigin::Client);
      return true; // server sent us an AckJoin when we're not registered
    }
  }

  // we're in the arena
  switch (packet.type) {
    case ServerPacket::Type::Pong:
      return true; // received a pong from the server

    case ServerPacket::Type::NotifyDelta: {
      if (!all(packet.arenaDelta)) return false;
      arena->applyDelta(*packet.arenaDelta);
      return true; // server sent us a NotifyDelta
    }

    case ServerPacket::Type::NotifyMessage: {
      if (!all(packet.stringData)) return false;
      if (packet.pid) {
        if (sky::Player *record = arena->getRecord(*packet.pid))
          messageLog.pushEntry(record->nickname + ": " + *packet.stringData);
        else messageLog.pushEntry("[unknown]: " + *packet.stringData);
      } else
        messageLog.pushEntry("[server]: " + *packet.stringData);
      return true; // server sent us a NotifyMessage
    }

    default:
      break;
  }

  return false;
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
    sky::PlayerDelta delta;
    delta.nickname = *settings.nickname;
    transmitServer(sky::ClientReqDelta(delta));
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
      transmitServer(ClientReqJoin(shared.settings.nickname));
      triedConnection = true;
      return;
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      bool success = false;
      if (const auto reception = telegraph.receive(event.packet)) {
        if (processPacket(*reception))
          success = true;
      }

      if (!success)
        appLog("Received malformed packet from server!",
               LogOrigin::Client);
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
  } else if (disconnecting) {
    f.drawText({400, 400}, {"Disconnecting..."}, 60, sf::Color::White);
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
          sky::ClientChat(std::string(*chatEntry.inputSignal)));
  }
}

void MultiplayerClient::signalClear() {
  chatEntry.signalClear();
}


