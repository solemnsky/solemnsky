#include "server.h"
#include "sky/event.h"
#include "util/methods.h"

/**
 * Server.
 */

Server::Server(const unsigned short port) :
    host(tg::HostType::Server, port),
    running(true) {
  appLog("Starting server on port " + std::to_string(port), LogOrigin::Server);
}

void Server::broadcastToClients(const sky::ServerPacket &packet) {
  telegraph.transmitMult(host, host.peers, packet);
}

void Server::broadcastToClientsExcept(const sky::PID pid,
                                      const sky::ServerPacket &packet) {
  telegraph.transmitMultPred(
      host, host.peers,
      [&](ENetPeer *peer) {
        if (sky::Player *player = playerFromPeer(peer))
          return player->pid != pid;
        else return false;
      }, packet);
}

void Server::transmitToClient(ENetPeer *const client,
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
      case ClientPacket::Type::ReqDelta: {
        appLog("got delta");
        if (packet.playerDelta->admin && not player->admin) return;
        sky::ArenaDelta delta = sky::ArenaDelta::Modify(
            player->pid, *packet.playerDelta);
        arena.applyDelta(delta);
        broadcastToClients(ServerPacket::NoteArenaDelta(delta));
        break;
      }

      case ClientPacket::Type::ReqTeamChange: {
        player->team = *packet.team;
        sky::PlayerDelta delta(*player);
        delta.team = *packet.team;
        broadcastToClients(sky::ServerPacket::NoteArenaDelta(
            sky::ArenaDelta::Modify(player->pid, delta)));
        break;
      }

      case ClientPacket::Type::ReqSpawn: {
        break;
      }

      case ClientPacket::Type::ReqKill: {
        break;
      }

      case ClientPacket::Type::NoteSkyDelta: {
        if (!arena.sky) return;
        arena.sky->applyDelta(*packet.skyDelta);
        break;
      }

      case ClientPacket::Type::Chat: {
        appLog("Chat \"" + player->nickname + "\": " + *packet.stringData);
        broadcastToClients(ServerPacket::Message(
            ServerMessage::Chat(player->pid, *packet.stringData)));
        break;
      }

      case ClientPacket::Type::Ping: {
        transmitToClient(client, ServerPacket::Pong());
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
      event.peer->data = &newPlayer;

      appLog("Client " + std::to_string(newPlayer.pid)
                 + " connected as \"" + *packet.stringData + "\".",
             LogOrigin::Server);

      transmitToClient(
          client, ServerPacket::AckJoin(
              newPlayer.pid, arena.captureInitializer()));
      broadcastToClientsExcept(
          newPlayer.pid, ServerPacket::NoteArenaDelta(
              ArenaDelta::Join(newPlayer)));
    }
  }
}

void Server::tick(float delta) {
  if (arena.sky) {
    arena.sky->tick(delta);
    broadcastToClients(sky::ServerPacket::NoteSkyDelta(
        arena.sky->collectDelta()));
  }

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
        appLog("Client disconnected, PID " + std::to_string(player->pid),
               LogOrigin::Server);

        broadcastToClientsExcept(
            player->pid, sky::ServerPacket::NoteArenaDelta(
                sky::ArenaDelta::Quit(player->pid)));

        arena.disconnectPlayer(*player);
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
  Server server(4242);
  sf::Clock clock;

  while (server.running) {
    server.tick(clock.restart().asSeconds());
    sf::sleep(sf::milliseconds(16));
  }
}
