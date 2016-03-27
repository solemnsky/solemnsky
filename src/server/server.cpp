#include "server.h"
#include "sky/event.h"
#include "util/methods.h"

/**
 * ServerLogger.
 */

void ServerLogger::registerPlayer(sky::Player &player) {
  player.data.push_back(nullptr);
}

void ServerLogger::unregisterPlayer(sky::Player &player) { }

ServerLogger::ServerLogger(sky::Arena &arena, Server &server) :
    Subsystem(arena),
    server(server) {
  for (auto &player : arena.players) registerPlayer(player.second);
}

void ServerLogger::onEvent(const ArenaEvent &event) {
  server.logArenaEvent(event);
}

/**
 * Server.
 */

void Server::logEvent(const ServerEvent &event) {
  appLog(event.print(), LogOrigin::Server);
}

void Server::logArenaEvent(const ArenaEvent &event) {
  logEvent(ServerEvent::Event(event));
}

Server::Server(const Port port,
               const sky::ArenaInitializer &initializer) :
    arena(initializer),
    logger(arena, *this),
    host(tg::HostType::Server, port),
    running(true) {
  logEvent(ServerEvent::Start(port, initializer.name));
}

void Server::sendToClients(const sky::ServerPacket &packet) {
  telegraph.transmit(
      host,
      [&](std::function<void(ENetPeer *const)> transmit) {
        for (auto const peer : host.peers) { transmit(peer); }
      }, packet);
}

void Server::sendToClientsExcept(const PID pid,
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

void Server::sendToClient(ENetPeer *const client,
                          const sky::ServerPacket &packet) {
  telegraph.transmit(host, client, packet);
}

sky::Player *Server::playerFromPeer(ENetPeer *peer) const {
  if (peer->data)
    return (sky::Player *) peer->data;
  else return nullptr;
}

void Server::processPacket(ENetPeer *client, const sky::ClientPacket &packet) {
  using namespace sky;
  // received a packet from a connected enet peer

  if (Player *player = playerFromPeer(client)) {
    // the player has joined the arena
    const std::string &pidString = std::to_string(player->pid);

    switch (packet.type) {
      case ClientPacket::Type::ReqPlayerDelta: {
        const PlayerDelta &delta = packet.playerDelta.get();
        if (delta.admin && not player->admin) return;

        sky::ArenaDelta arenaDelta = sky::ArenaDelta::Delta(
            player->pid, delta);
        arena.applyDelta(arenaDelta);
        sendToClients(ServerPacket::DeltaArena(arenaDelta));
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
        sky::ServerMessage message =
            ServerMessage::Chat(player->pid, packet.stringData.get());
        logArenaEvent(ArenaEvent::Chat(
            player->nickname, packet.stringData.get()));
        sendToClients(sky::ServerPacket::Message(message));
        break;
      }

      case ClientPacket::Type::Ping: {
        sendToClient(client, ServerPacket::Pong());
        break;
      }

      default:
        break;
    }
  } else {
    // client is still connecting, we need to do a ReqJoin / AckJoin
    // handshake, distribute an ArenaDelta to the other clients, and attach
    // a sky::Player to the enet peer
    if (packet.type == ClientPacket::Type::ReqJoin) {
      sky::Player &newPlayer = arena.connectPlayer(*packet.stringData);
      client->data = &newPlayer;

      logEvent(ServerEvent::Connect(newPlayer.nickname));
      sendToClient(
          client, ServerPacket::Init(
              newPlayer.pid, arena.captureInitializer(), {}));
      sendToClientsExcept(
          newPlayer.pid, ServerPacket::DeltaArena(
              ArenaDelta::Join(newPlayer.captureInitializer())));
    }
  }
}

void Server::tick(float delta) {
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
      if (sky::Player *player = playerFromPeer(event.peer)) {

        logEvent(ServerEvent::Disconnect(player->nickname));
        sendToClientsExcept(
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

int main() {
  Server server(4242, sky::ArenaInitializer("my arena"));
  sf::Clock clock;

  while (server.running) {
    server.tick(clock.restart().asSeconds());
    sf::sleep(sf::milliseconds(16));
  }
}
