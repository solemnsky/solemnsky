#include "multiplayershared.h"

/**
 * MultiplayerConnection.
 */

void MultiplayerConnection::processPacket(const sky::ServerPacket &packet) {
  using namespace sky;

  if (!myPlayer) {
    // waiting for the arena connection request to be accepted
    if (packet.type == ServerPacket::Type::Init) {
      arena.emplace(*packet.init);
      myPlayer = arena.getPlayer(*packet.pid);
      appLog("Joined arena!", LogOrigin::Client);
      connected = true;
      if (host.peers.size() == 1) {
        eventLog.push_back(sky::ClientEvent::Connect(
            "(haven't implemented server names lol)",
            tg::printAddress(host.peers[0]->address)));
      }
    }
    return;
  }

  // we're in the arena
  switch (packet.type) {
    case ServerPacket::Type::Pong: {
      break;
    }

    case ServerPacket::Type::Message: {
      switch (packet.message->type) {
        case ServerMessage::Type::Chat: {
          if (sky::Player *player = arena.getPlayer(*packet.message->from)) {
            eventLog.push_back(sky::ClientEvent::Chat(
                player->nickname, packet.message->contents));
          } else {
            eventLog.push_back(sky::ClientEvent::Chat(
                "<unknown>", packet.message->contents));
          }
          break;
        }
        case ServerMessage::Type::Broadcast: {
          eventLog.push_back(sky::ClientEvent::Broadcast(
              packet.message->contents));
          break;
        }
      }
      break;
    }

    case ServerPacket::Type::Delta: {
      if (optional<sky::ClientEvent> event =
          arena.applyDelta(*packet.delta)) {
        eventLog.push_back(*event);
      }
      break;
    }

    case ServerPacket::Type::DeltaSky: {
      if (arena.sky) arena.sky->applyDelta(*packet.skyDelta);
      break;
    }

    default:
      break;
  }
}

MultiplayerConnection::MultiplayerConnection(
    ClientShared &shared,
    const std::string &serverHostname,
    const unsigned short serverPort) :
    shared(shared),
    server(nullptr),
    telegraph(),
    askedConnection(false),
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
  if (disconnected) return {};

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
        processPacket(*reception);
        return *reception;
      }
    }
  }

  return {};
}

void MultiplayerConnection::disconnect() {
  if (server) {
    host.disconnect(server);
    disconnecting = true;
    disconnectTimeout.reset();
  } else {
    disconnected = true;
  }
}

void MultiplayerConnection::requestTeamChange(sky::Team team) {
  if (myPlayer) {
    sky::PlayerDelta delta(*myPlayer);
    delta.team = team;
    transmit(sky::ClientPacket::ReqPlayerDelta(delta));
  }
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

