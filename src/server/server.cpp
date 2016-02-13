#include "server.h"
#include "util/methods.h"

/**
 * Server.
 */

Server::Server(const unsigned short port) :
    host(tg::HostType::Server, port),
    telegraph(sky::pk::clientPacketPack, sky::pk::serverPacketPack) {
  appLog("Starting server on port " + std::to_string(port),
         LogOrigin::Server);
}

/**
 * Packet processing subroutines.
 */

void Server::broadcastToClients(const sky::prot::ServerPacket &packet) {
  telegraph.transmitMult(host, host.getPeers(), packet);
}

void Server::broadcastToClientsExcept(const sky::PID pid,
                                      const sky::prot::ServerPacket &packet) {
  telegraph.transmitMultPred(
      host, host.getPeers(),
      [&](ENetPeer *peer) {
        return recordFromPeer(peer).pid == pid;
      }, packet);
}

sky::PlayerRecord &Server::recordFromPeer(ENetPeer *peer) const {
  return *((sky::PlayerRecord *) peer->data);
}

void Server::processPacket(ENetPeer *client,
                           const sky::prot::ClientPacket &packet) {
  // received a packet from a connected peer

  using namespace sky::prot;
  sky::PlayerRecord &record = recordFromPeer(client);

  if (packet.type == ClientPacket::Type::Ping) {
    // we always send a pong to ping requests
    telegraph.transmit(host, client, ServerPong());
  }

  const std::string &pidString = std::to_string(record.pid);

  if (record.connected) {
    // if the client isn't connected, we just want a ReqConnection before
    // anything else...
    if (packet.type == ClientPacket::Type::ReqConnection) {
      appLog(
          "client " + pidString + " joining server with nick "
              + *packet.stringData,
          LogOrigin::Server);
      record.nickname = *packet.stringData;
    }
  } else {
    switch (packet.type) {
      case sky::prot::ClientPacket::Type::Chat: {
        appLog(
            "client " + pidString + " says: " + *packet
                .stringData,
            LogOrigin::Server);
        break;
      }
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
      sky::PlayerRecord &record = arena.connectPlayer();
      appLog("Client connected, PID " + std::to_string(record.pid));
      event.peer->data = &record;

      break;
    }
    case ENET_EVENT_TYPE_DISCONNECT: {
      sky::PlayerRecord &record = recordFromPeer(event.peer);
      appLog("Client disconnected, PID " + std::to_string(record.pid));
      arena.disconnectPlayer(record);
      break;
    }
    case ENET_EVENT_TYPE_RECEIVE: {
      appLog("Received packet.");
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
