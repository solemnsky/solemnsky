#include "server.h"

/**
 * ServerTelegraphy.
 */

ServerTelegraphy::ServerTelegraphy(
    tg::Host &host, tg::Telegraph<sky::ClientPacket> &telegraph) :
    host(host), telegraph(telegraph) { }

sky::Player *ServerTelegraphy::playerFromPeer(ENetPeer *peer) const {
  if (peer->data) return (sky::Player *) peer->data;
  else return nullptr;
}

void ServerTelegraphy::sendToClients(const sky::ServerPacket &packet) {
  telegraph.transmit(
      host,
      [&](std::function<void(ENetPeer *const)> transmit) {
        for (auto const peer : host.peers) { transmit(peer); }
      }, packet);
}

void ServerTelegraphy::sendToClientsExcept(const PID pid,
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

void ServerTelegraphy::sendToClient(ENetPeer *const client,
                                    const sky::ServerPacket &packet) {
  telegraph.transmit(host, client, packet);
}

/**
 * Server.
 */

Server::Server(ServerTelegraphy &telegraphy,
               sky::Arena &arena, sky::Sky &sky) :
    Subsystem(arena),
    telegraphy(telegraphy),
    sky(sky) { }

/**
 * ServerExec.
 */

void ServerExec::processPacket(ENetPeer *client,
                               const sky::ClientPacket &packet) {
  using namespace sky;
  // received a packet from a connected enet peer

  if (Player *player = telegraphy.playerFromPeer(client)) {
    // the player has joined the arena
    switch (packet.type) {
      case ClientPacket::Type::ReqPlayerDelta: {
        const PlayerDelta &delta = packet.playerDelta.get();
        if (delta.admin && not player->admin) return;

        sky::ArenaDelta arenaDelta = sky::ArenaDelta::Delta(
            player->pid, delta);
        arena.applyDelta(arenaDelta);
        telegraphy.sendToClients(ServerPacket::DeltaArena(arenaDelta));
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
        logArenaEvent(ArenaEvent::Chat(
            player->nickname, packet.stringData.get()));
        telegraphy.sendToClients(sky::ServerPacket::Chat(
            player->pid, packet.stringData.get()));
        break;
      }

      case ClientPacket::Type::Ping: {
        telegraphy.sendToClient(client, ServerPacket::Pong());
        break;
      }

      default:
        break;
    }

    server->onPacket(client, *player, packet);
  } else {
    // client is still connecting, we need to do a ReqJoin / AckJoin
    // handshake, distribute an ArenaDelta to the other clients, and attach
    // a sky::Player to the enet peer
    if (packet.type == ClientPacket::Type::ReqJoin) {
      sky::Player &newPlayer = arena.connectPlayer(*packet.stringData);
      client->data = &newPlayer;

      logEvent(ServerEvent::Connect(newPlayer.nickname));
      telegraphy.sendToClient(
          client, ServerPacket::Init(
              newPlayer.pid, arena.captureInitializer(), {}));
      telegraphy.sendToClientsExcept(
          newPlayer.pid, ServerPacket::DeltaArena(
              ArenaDelta::Join(newPlayer.captureInitializer())));
    }
  }
}

void ServerExec::logEvent(const ServerEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Server);
}

void ServerExec::logArenaEvent(const ArenaEvent &event) {
  logEvent(ServerEvent::Event(event));
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
      if (sky::Player *player = telegraphy.playerFromPeer(event.peer)) {
        logEvent(ServerEvent::Disconnect(player->nickname));
        telegraphy.sendToClientsExcept(
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
        ServerTelegraphy &, sky::Arena &, sky::Sky &)> server) :
    host(tg::HostType::Server, port),
    telegraphy(host, telegraph),
    arena(arena),
    sky(this->arena, sky),
    server(server(telegraphy, this->arena, this->sky)) {
  logEvent(ServerEvent::Start(port, arena.name));
}

void ServerExec::run() {
  sf::Clock clock;
  while (running) {
    tick(clock.restart().asSeconds());
    sf::sleep(sf::milliseconds(16));
  }
}

