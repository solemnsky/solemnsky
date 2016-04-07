#include "server.h"

/**
 * ServerShared.
 */

ServerShared::ServerShared(
    tg::Host &host, tg::Telegraph<sky::ClientPacket> &telegraph,
    ServerExec &exec) :
    exec(exec), host(host), telegraph(telegraph) { }

sky::Player *ServerShared::playerFromPeer(ENetPeer *peer) const {
  if (peer->data) return (sky::Player *) peer->data;
  else return nullptr;
}

void ServerShared::sendToClients(const sky::ServerPacket &packet) {
  telegraph.transmit(
      host,
      [&](std::function<void(ENetPeer *const)> transmit) {
        for (auto const peer : host.peers) { transmit(peer); }
      }, packet);
}

void ServerShared::sendToClientsExcept(const PID pid,
                                       const sky::ServerPacket &packet) {
  telegraph.transmit(
      host,
      [&](std::function<void(ENetPeer *const)> transmit) {
        for (auto const peer : host.peers) {
          if (sky::Player *player = playerFromPeer(peer)) {
            if (player->pid != pid) transmit(peer);
          }
        }
      }, packet);
}

void ServerShared::sendToClient(ENetPeer *const client,
                                const sky::ServerPacket &packet) {
  telegraph.transmit(host, client, packet);
}

void ServerShared::rconResponse(ENetPeer *const client,
                                const std::string &response) {
  sendToClient(client, sky::ServerPacket::RCon(response));
  logEvent(ServerEvent::RConOut(response));
}

void ServerShared::logEvent(const ServerEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Server);
}

void ServerShared::logArenaEvent(const sky::ArenaEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Engine);
}

/**
 * Server.
 */

Server::Server(ServerShared &shared,
               sky::Arena &arena, sky::Sky &sky) :
    Subsystem(arena),
    shared(shared),
    sky(sky) { }

/**
 * ServerLogger.
 */

void ServerLogger::registerPlayer(sky::Player &player) {
  player.data.push_back(nullptr);
}

void ServerLogger::unregisterPlayer(sky::Player &player) {

}

void ServerLogger::onEvent(const sky::ArenaEvent &event) {
  shared.logArenaEvent(event);
}

ServerLogger::ServerLogger(ServerShared &shared, sky::Arena &arena) :
    sky::Subsystem(arena), shared(shared) {
  arena.forPlayers([&](sky::Player &p) { registerPlayer(p); });
}

/**
 * ServerExec.
 */

void ServerExec::processPacket(ENetPeer *client,
                               const sky::ClientPacket &packet) {
  using namespace sky;

  if (Player *player = shared.playerFromPeer(client)) {
    // the player is in the arena

    switch (packet.type) {
      case ClientPacket::Type::ReqPlayerDelta: {
        const PlayerDelta &delta = packet.playerDelta.get();
        if (delta.admin && not player->admin) return;

        sky::ArenaDelta arenaDelta = sky::ArenaDelta::Delta(
            player->pid, delta);
        arena.applyDelta(arenaDelta);
        shared.sendToClients(ServerPacket::DeltaArena(arenaDelta));
        break;
      }

      case ClientPacket::Type::ReqSkyDelta: {
        break;
      }

      case ClientPacket::Type::ReqSpawn: {
        break;
      }

      case ClientPacket::Type::ReqKill: {
        break;
      }

      case ClientPacket::Type::Chat: {
        shared.sendToClients(sky::ServerPacket::Chat(
            player->pid, packet.stringData.get()));
        break;
      }

      case ClientPacket::Type::Ping: {
        shared.sendToClient(client, ServerPacket::Pong());
        break;
      }

      case ClientPacket::Type::RCon: {
        shared.logEvent(ServerEvent::RConIn(packet.stringData.get()));
        break;
      }

      default:
        break;
    }

    server->onPacket(client, *player, packet);

  } else {
    // client hasn't joined the arena yet

    if (packet.type == ClientPacket::Type::ReqJoin) {
      sky::Player &newPlayer = arena.connectPlayer(packet.stringData.get());
      client->data = &newPlayer;

      shared.logEvent(ServerEvent::Connect(newPlayer.nickname));
      shared.sendToClient(
          client, ServerPacket::Init(
              newPlayer.pid, arena.captureInitializer(), {}));
      shared.sendToClientsExcept(
          newPlayer.pid, ServerPacket::DeltaArena(
              ArenaDelta::Join(newPlayer.captureInitializer())));
    }
  }
}


void ServerExec::tick(float delta) {
  static ENetEvent event;
  event = host.poll();
  switch (event.type) {
    case ENET_EVENT_TYPE_NONE:
      break;
    case ENET_EVENT_TYPE_CONNECT: {
      appLog("Client connecting...");
      event.peer->data = nullptr;
      break;
    }
    case ENET_EVENT_TYPE_DISCONNECT: {
      if (sky::Player *player = shared.playerFromPeer(event.peer)) {
        shared.logEvent(ServerEvent::Disconnect(player->nickname));
        shared.sendToClientsExcept(
            player->pid, sky::ServerPacket::DeltaArena(
                sky::ArenaDelta::Quit(player->pid)));

        arena.quitPlayer(*player);
        event.peer->data = nullptr;
      }
      break;
    }
    case ENET_EVENT_TYPE_RECEIVE: {
      if (const auto &reception = telegraph.receive(event.packet))
        processPacket(event.peer, *reception);
      break;
    }
  }

  uptime += delta;
}

ServerExec::ServerExec(
    const Port port,
    const sky::ArenaInitializer &arena,
    const sky::SkyInitializer &sky,
    std::function<std::unique_ptr<Server>(
        ServerShared &, sky::Arena &, sky::Sky &)> server) :
    host(tg::HostType::Server, port),
    shared(host, telegraph, *this),
    arena(arena),
    sky(this->arena, sky),
    server(server(shared, this->arena, this->sky)),
    logger(shared, this->arena),
    running(true) {
  shared.logEvent(ServerEvent::Start(port, arena.name));
}

void ServerExec::run() {
  sf::Clock clock;
  while (running) {
    tick(clock.restart().asSeconds());
    sf::sleep(sf::milliseconds(16));
  }
}

