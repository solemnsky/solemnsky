#include "server.h"
#include "util/methods.h"

/**
 * Server.
 */

Server::Server(const unsigned short port) :
    host(tg::HostType::Server, port),
    telegraph(sky::clientPacketPack, sky::serverPacketPack) {
  appLog("Starting server on port " + std::to_string(port),
         LogOrigin::Server);
}

/**
 * Packet processing subroutines.
 */

void Server::broadcastToClients(const sky::ServerPacket &packet) {
  appLog("To all: " + packet.dump());
  telegraph.transmitMult(host, host.peers, packet);
}

void Server::broadcastToClientsExcept(const sky::PID pid,
                                      const sky::ServerPacket &packet) {
  appLog("To all except "
             + std::to_string(pid) + ": " + packet.dump());
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
  std::string clientDesc = "[unconnected]";
  if (sky::Player *player = playerFromPeer(client))
    clientDesc = std::to_string(player->pid);

  appLog(
      "To client " + clientDesc + ": "
          + packet.dump());
  telegraph.transmit(host, client, packet);
}

sky::Player *Server::playerFromPeer(ENetPeer *peer) const {
  if (peer->data)
    return (sky::Player *) peer->data;
  else return nullptr;
}

void Server::processPacket(ENetPeer *client,
                           const sky::ClientPacket &packet) {
  // received a packet from a connected peer

  using namespace sky;

  if (Player *player = playerFromPeer(client)) {
    const std::string &pidString = std::to_string(player->pid);
    appLog("Client " + pidString + ": " + packet.dump());

    switch (packet.type) {
      case ClientPacket::Type::ReqDelta: {
        sky::ArenaDelta delta;
        delta.playerDelta = std::pair<sky::PID, sky::PlayerDelta>(
            player->pid, *packet.playerDelta
        );
        arena.applyDelta(delta);
        broadcastToClients(ServerNotifyDelta(delta));
        break;
      }
      case ClientPacket::Type::Chat: {
        broadcastToClients(ServerNotifyMessage(*packet.stringData,
                                               player->pid));
        break;
      }
      case ClientPacket::Type::Ping: {
        transmitToClient(client, ServerPong());
        break;
      }
      default:
        break;
    }
  } else {
    // client is still connecting, we need to do a ReqJoin / AckJoin
    // handshake and then distribute an ArenaDelta to the other clients
    if (packet.type == ClientPacket::Type::ReqJoin) {
      sky::Player &newPlayer = arena.connectPlayer();
      newPlayer.nickname = *packet.stringData;
      event.peer->data = &newPlayer;

      appLog("Client " + std::to_string(newPlayer.pid)
                 + " entering in arena as \"" + *packet.stringData + "\".",
             LogOrigin::Server);
      transmitToClient(
          client, ServerAckJoin(newPlayer.pid, arena.captureInitializer()));

      sky::ArenaDelta delta;
      delta.playerJoin = newPlayer;
      broadcastToClientsExcept(newPlayer.pid, ServerNotifyDelta(delta));
    }
  }
}

/**
 * Server tick.
 */

void Server::tick(float delta) {
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
        sky::ArenaDelta arenaDelta;
        arenaDelta.playerQuit = player->pid;
        broadcastToClientsExcept(
            player->pid,
            sky::ServerNotifyDelta(arenaDelta));
        arena.disconnectPlayer(*player);
      }
      break;
    }
    case ENET_EVENT_TYPE_RECEIVE: {
      processPacket(event.peer, telegraph.receive(event.packet));
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
