#include "multiplayershared.h"

/**
 * MultiplayerSubsystem.
 */

void MultiplayerSubsystem::registerPlayer(sky::Player &player) {
  player.data.push_back(nullptr);
}

void MultiplayerSubsystem::unregisterPlayer(sky::Player &player) {

}

void MultiplayerSubsystem::onEvent(const ArenaEvent &event) {
  connection.logArenaEvent(event);
}

MultiplayerSubsystem::MultiplayerSubsystem(
    sky::Arena &arena,
    MultiplayerConnection &connection) :
    sky::Subsystem(arena), connection(connection) {
  arena.forPlayers([&](sky::Player &player) {
    registerPlayer(player);
  });
}

/**
 * MultiplayerConnection.
 */

void MultiplayerConnection::processPacket(const sky::ServerPacket &packet) {
  using namespace sky;

  if (!myPlayer) {
    // waiting for the arena connection request to be accepted
    if (packet.type == ServerPacket::Type::Init) {
      initializeArena(
          packet.pid.get(), packet.arenaInit.get(), packet.skyInit.get());
      appLog("Joined arena!", LogOrigin::Client);

      logEvent(ClientEvent::Connect(
          "(haven't implemented server names lol)",
          tg::printAddress(host.peers[0]->address)));
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
          if (sky::Player *player = arena->getPlayer(
              packet.message->from.get())) {
            logArenaEvent(ArenaEvent::Chat(
                player->nickname, packet.message->contents));
          }
          break;
        }
        case ServerMessage::Type::Broadcast: {
          logArenaEvent(ArenaEvent::Broadcast(
              packet.message->contents));
          break;
        }
      }
      break;
    }

    case ServerPacket::Type::DeltaArena: {
      arena->applyDelta(packet.arenaDelta.get());
      break;
    }

    case ServerPacket::Type::DeltaSky: {
      break;
    }

    default:
      break;
  }
}

void MultiplayerConnection::logEvent(const ClientEvent &event) {
  eventLog.push_back(event);
}

void MultiplayerConnection::logArenaEvent(const ArenaEvent &event) {
  logEvent(ClientEvent::Event(event));
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
    myPlayer(nullptr) {
  host.connect(serverHostname, serverPort);
}

void MultiplayerConnection::initializeArena(
    const PID pid,
    const sky::ArenaInitializer &arenaInit,
    const sky::SkyInitializer &skyInit) {
  arena.emplace(arenaInit);
  sky.emplace(arena.get(), skyInit);
  multiplayerSubsystem.emplace(arena.get(), *this);
  myPlayer = arena->getPlayer(pid);
}

void MultiplayerConnection::transmit(const sky::ClientPacket &packet) {
  if (server) telegraph.transmit(host, server, packet);
}

void MultiplayerConnection::poll(const float delta) {
  if (disconnected) return;

  event = host.poll();
  if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
    server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    disconnected = true;
    return;
  }

  if (disconnecting) {
    if (disconnectTimeout.cool(delta)) {
      appLog("Disconnected from unresponsive server!", LogOrigin::Client);
      disconnected = true;
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
    if (!askedConnection) {
      // we have a link but haven't sent an arena connection request
      appLog("Asking to join arena...", LogOrigin::Client);
      transmit(sky::ClientPacket::ReqJoin(shared.settings.nickname));
      askedConnection = true;
      return;
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      if (const auto reception = telegraph.receive(event.packet))
        processPacket(*reception);
    }
  }
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

void MultiplayerConnection::requestTeamChange(const sky::Team team) {
  if (myPlayer) {
    sky::PlayerDelta delta = myPlayer->zeroDelta();
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

