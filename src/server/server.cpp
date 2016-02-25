#include "server.h"
#include "util/methods.h"

/**
 * Server.
 */

Server::Server(const unsigned short port) :
    host(tg::HostType::Server, port),
    telegraph(sky::ClientPacketPack(), sky::ServerPacketPack()),
    running(true) {
  appLog("Starting server on port " + std::to_string(port),
         LogOrigin::Server);
}

/**
 * Packet processing subroutines.
 */

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

bool Server::processPacket(ENetPeer *client, const sky::ClientPacket &packet) {
  // received a packet from a connected peer
  using namespace sky;

  if (Player *player = playerFromPeer(client)) {
    const std::string &pidString = std::to_string(player->pid);

    switch (packet.type) {
      case ClientPacket::Type::ReqDelta: {
        if (!all(packet.playerDelta))
          return false;

        sky::ArenaDelta delta;
        delta.player = std::pair<sky::PID, sky::PlayerDelta>(
            player->pid, *packet.playerDelta
        );
        arena.applyDelta(delta);
        broadcastToClients(ServerNotifyDelta(delta));
        return true; // client send a ReqDelta
      }

      case ClientPacket::Type::Chat: {
        if (!all(packet.stringData))
          return false;

        appLog("Chat \"" + player->nickname + "\": " + *packet.stringData);
        broadcastToClients(ServerNotifyMessage(*packet.stringData,
                                               player->pid));
        return true; // client sent a Chat
      }

      case ClientPacket::Type::Ping: {
        transmitToClient(client, ServerPong());
        return true; // client sent a Ping
      }

      default:
        break;
    }
  } else {
    // client is still connecting, we need to do a ReqJoin / AckJoin
    // handshake and then distribute an ArenaDelta to the other clients
    if (packet.type == ClientPacket::Type::ReqJoin) {
      if (!all(packet.stringData)) return false;

      sky::Player &newPlayer = arena.connectPlayer();
      newPlayer.nickname = *packet.stringData;
      event.peer->data = &newPlayer;

      appLog("Client " + std::to_string(newPlayer.pid)
                 + " connected as \"" + *packet.stringData + "\".",
             LogOrigin::Server);
      transmitToClient(
          client, ServerAckJoin(newPlayer.pid, arena.captureInitializer()));

      sky::ArenaDelta delta;
      delta.join = newPlayer;
      broadcastToClientsExcept(newPlayer.pid, ServerNotifyDelta(delta));

      return true; // unregistered client sent a ReqJoin
    }
  }

  return false;
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
        arenaDelta.quit = player->pid;
        broadcastToClientsExcept(
            player->pid,
            sky::ServerNotifyDelta(arenaDelta));
        arena.disconnectPlayer(*player);
      }
      break;
    }
    case ENET_EVENT_TYPE_RECEIVE: {
      bool success = false;
      if (const auto &reception = telegraph.receive(event.packet)) {
        if (processPacket(event.peer, *reception)) success = true;
      }

      if (!success) {
        // something went wrong with the packet process
        if (sky::Player *player = playerFromPeer(event.peer))
          appLog("Received malformed packet from client "
                     + std::to_string(player->pid) + "!",
                 LogOrigin::Server);
        else
          appLog(
              "Received malformed packet from unregistered client!",
              LogOrigin::Server);
      }
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
