#include "multiplayerclient.h"
#include "util/methods.h"

MultiplayerClient::MultiplayerClient(ClientShared &state,
                                     const std::string &serverHostname,
                                     const unsigned short serverPort) :
    Game(state, "multiplayer"),

    quitButton({100, 50}, "quit tutorial"),
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
  appLog("Transmitting: " + packet.dump());
  if (server) telegraph.transmit(host, server, packet);
}

void MultiplayerClient::handleNetwork(const sky::ServerPacket &packet) {
  // at this point the whole enet / arena connection has established and
  // we're not disconnecting.

  using namespace sky;

  switch (packet.type) {
    case ServerPacket::Type::Pong:
      break;
    case ServerPacket::Type::NotifyDelta: {
      arena->applyDelta(*packet.arenaDelta);
      break;
    }
    case ServerPacket::Type::NotifyMessage: {
      if (packet.pid) {
        if (sky::Player *record = arena->getRecord(*packet.pid))
          messageLog.pushEntry(record->nickname + ": " + *packet.stringData);
        else messageLog.pushEntry("[unknown]: " + *packet.stringData);
      } else
        messageLog.pushEntry("[server]: " + *packet.stringData);
      break;
    }
    default:
      break;
  }
}

/**
 * Game interface.
 */

void MultiplayerClient::onLooseFocus() {
  quitButton.reset();
  chatEntry.reset();
}

void MultiplayerClient::onFocus() {

}

void MultiplayerClient::doExit() {
  if (!server) {
    concluded = true;
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
  quitButton.tick(delta);
  chatEntry.tick(delta);
  messageLog.tick(delta);

  using namespace sky;

  event = host.poll();
  if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
    server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    concluded = true;
    return;
  }

  if (disconnecting) {
    if (disconnectTimeout.cool(delta)) {
      appLog("Disconnecting from unresponsive server!", LogOrigin::Client);
      concluded = true;
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
      transmitServer(ClientReqJoin(shared.settings.preferredNickname));
      triedConnection = true;
      return;
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      const ServerPacket &packet = telegraph.receive(event.packet);
      appLog("Receiving: " + packet.dump());

      if (arena) {
        // in the arena
        handleNetwork(packet);
      } else {
        // waiting for the arena connection request to be accepted
        if (packet.type == ServerPacket::Type::AckJoin) {
          arena.emplace(*packet.arenaInitializer);
          myRecord = arena->getRecord(*packet.pid);
          appLog("Joined arena!", LogOrigin::Client);
        }
      }
    }
  }

}

void MultiplayerClient::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});

  quitButton.render(f);
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
  if (quitButton.handle(event)) return true;
  if (event.type == sf::Event::EventType::KeyPressed) {
    if (event.key.code == sf::Keyboard::Return) {
      chatEntry.focus();
    }
  }
  return messageLog.handle(event);
}

void MultiplayerClient::signalRead() {
  if (quitButton.clickSignal) doExit();
  if (chatEntry.inputSignal) {
    if (arena)
      transmitServer(
          sky::ClientChat(std::string(*chatEntry.inputSignal)));
  }
}

void MultiplayerClient::signalClear() {
  quitButton.signalClear();
  chatEntry.signalClear();
}


