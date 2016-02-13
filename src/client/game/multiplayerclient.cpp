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
    telegraph(sky::pk::serverPacketPack, sky::pk::clientPacketPack),
    pingCooldown(5),
    triedConnection(false),
    arenaConnected(false),
    disconnecting(false),
    disconnectTimeout(1) {
  host.connect(serverHostname, serverPort);
}

/**
 * Networking submethods.
 */

void MultiplayerClient::transmitServer(const sky::prot::ClientPacket &packet) {
  if (server)
    telegraph.transmit(host, server, packet);
}

void MultiplayerClient::handleNetwork(const sky::prot::ServerPacket &packet) {
  // at this point the whole enet / arena connection has established and
  // we're not disconnecting.

  using namespace sky::prot;

  switch (packet.type) {
    case ServerPacket::Type::Pong:
      break;
    case ServerPacket::Type::NotifyConnection: {
      arena.applyConnection(*packet.record);
      break;
    }
    case ServerPacket::Type::NotifyRecordDelta: {
      arena.applyRecordDelta(*packet.pid, *packet.recordDelta);
    }
    case ServerPacket::Type::NotifyDisconnection: {
      arena.applyDisconnection(*packet.pid);
    }
    case ServerPacket::Type::NotifyMessage: {
      if (packet.pid) {
        messageLog.pushEntry(
            arena.getRecord(*packet.pid)->nickname +
                ": " + *packet.stringData);
      } else {
        messageLog.pushEntry("[server]: " + *packet.stringData);
      }
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

  using namespace sky::prot;

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
      transmitServer(ClientReqConnection(shared.settings.preferredNickname));
      appLog("Joining arena...", LogOrigin::Client);
      triedConnection = true;
      return;
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      const ServerPacket &packet = telegraph.receive(event.packet);

      if (arenaConnected) {
        handleNetwork(packet);
      } else {
        // waiting for the arena connection request to be accepted
        if (packet.type == ServerPacket::Type::AcceptConnection) {
          arena = *packet.arena;
          myRecord = arena.getRecord(*packet.pid);
          arenaConnected = true;
          appLog("Joined arena with "
                     + std::to_string(arena.playerRecords.size())
                     + " other players!",
                 LogOrigin::Client);
        }
      }
    }
  }

}

void MultiplayerClient::render(ui::Frame &f) {
  quitButton.render(f);
  chatEntry.render(f);
  messageLog.render(f);

  if (!server) {
    f.drawText({400, 400}, {"Connecting..."}, 60, sf::Color::White);
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
    if (arenaConnected)
      transmitServer(
          sky::prot::ClientChat(std::string(*chatEntry.inputSignal)));
  }
}

void MultiplayerClient::signalClear() {
  quitButton.signalClear();
  chatEntry.signalClear();
}


