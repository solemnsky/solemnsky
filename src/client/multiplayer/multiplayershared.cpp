#include "multiplayershared.h"

/**
 * MultiplayerConnection.
 */

bool MultiplayerConnection::processPacket(const sky::ServerPacket &packet) {
  using namespace sky;

  if (!myPlayer) {
    // waiting for the arena connection request to be accepted

    if (packet.type == ServerPacket::Type::AckJoin) {
      arena.applyInitializer(*packet.arenaInitializer);
      myPlayer = arena.getPlayer(*packet.pid);
      appLog("Joined arena!", LogOrigin::Client);
      connected = true;
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
      switch (packet.message->type) {
        case ServerMessage::Type::Chat: {
          messageLog.push_back("[chat] " + *packet.message->from +
              ": " + packet.message->contents);
          break;
        }
        case ServerMessage::Type::Broadcast: {
          messageLog.push_back("[server] : " + packet.message->contents);
          break;
        }
      }
      return true;
    }

    case ServerPacket::Type::NoteArenaDelta: {
      arena.applyDelta(*packet.arenaDelta);
      return true;
    }

    case ServerPacket::Type::NoteSkyDelta: {
      if (arena.sky) arena.sky->applyDelta(*packet.skyDelta);
      return true;
    }

    default:
      break;
  }
  return false;
}

MultiplayerConnection::MultiplayerConnection(
    ClientShared &shared,
    const std::string &serverHostname,
    const unsigned short serverPort) :

    shared(shared),
    server(nullptr),
    telegraph(),
    askedConnection(true),
    disconnectTimeout(5),
    host(tg::HostType::Client),
    disconnected(false), disconnecting(false),
    connected(false),
    myPlayer(nullptr) {
  host.connect(serverHostname, serverPort);
}

void MultiplayerConnection::transmit(const sky::ClientPacket &packet) {
  if (server) telegraph.transmit(host, server, packet);
}

optional<sky::ServerPacket> MultiplayerConnection::poll(const float delta) {
  event = host.poll();
  if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
    server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    disconnected = true;
    return {};
  }

  if (disconnecting) {
    if (disconnectTimeout.cool(delta)) {
      appLog("Disconnected from unresponsive server!", LogOrigin::Client);
      disconnected = true;
    }
    return {};
  }

  if (!server) {
    // still trying to connect to the server...
    if (event.type == ENET_EVENT_TYPE_CONNECT) {
      server = event.peer;
      appLog("Connected to server!", LogOrigin::Client);
    }
  } else {
    // connected
    if (!askedConnection) {
      // we have a link but haven't sent an arena connection request
      appLog("Asking to join arena...", LogOrigin::Client);
      transmit(sky::ClientPacket::ReqJoin(shared.settings.nickname));
      askedConnection = true;
      return {};
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      if (const auto reception = telegraph.receive(event.packet)) {
        // connected to enet, receiving a protocol packet
        if (processPacket(*reception)) return *reception;
      }
    }
  }

  return {};
}

void MultiplayerConnection::disconnect() {
  host.disconnect(server);
  disconnectTimeout.reset();
}

/**
 * MultiplayerView.
 */


MultiplayerView::MultiplayerView(
    sky::ArenaMode target,
    ClientShared &shared,
    MultiplayerConnection &connection) :
    target(target),
    shared(shared),
    connection(connection) { }

