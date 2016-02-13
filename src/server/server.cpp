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
  appLog("To all: " + packet.dump());
  telegraph.transmitMult(host, host.peers, packet);
}

void Server::broadcastToClientsExcept(const sky::PID pid,
                                      const sky::prot::ServerPacket &packet) {
  appLog("To all except "
             + std::to_string(pid) + ": " + packet.dump());
  telegraph.transmitMultPred(
      host, host.peers,
      [&](ENetPeer *peer) {
        return recordFromPeer(peer).pid != pid;
      }, packet);
}

void Server::transmitToClient(ENetPeer *const client,
                              const sky::prot::ServerPacket &packet) {
  appLog(
      "To client " + std::to_string(recordFromPeer(client).pid) + ": "
          + packet.dump());
  telegraph.transmit(host, client, packet);
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
    transmitToClient(client, ServerPong());
    return;
  }

  const std::string &pidString = std::to_string(record.pid);
  appLog("Client " + pidString + ": " + packet.dump());

  if (!record.connected) {
    // if the client isn't connected, we just want a ReqConnection before
    // anything else...
    if (packet.type == ClientPacket::Type::ReqConnection) {
      appLog("Client " + pidString + " entering in arena as \""
                 + *packet.stringData + "\".",
             LogOrigin::Server);
      record.nickname = *packet.stringData;
      record.connected = true;
      transmitToClient(client, ServerAcceptConnection(record.pid, arena));

      sky::PlayerRecordDelta delta;
      delta.connected = true;
      delta.nickname = record.nickname;
      broadcastToClientsExcept(record.pid, ServerNotifyRecordDelta(delta));
    }
  } else {
    switch (packet.type) {
      case sky::prot::ClientPacket::Type::Chat: {
        broadcastToClients(ServerNotifyMessage(*packet.stringData, record.pid));
        break;
      }
      case ClientPacket::Type::Ping: {
        transmitToClient(client, ServerPong());
        break;
      }
      default:
        break;
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
      broadcastToClientsExcept(record.pid,
                               sky::prot::ServerNotifyConnection(record.pid));
      break;
    }
    case ENET_EVENT_TYPE_DISCONNECT: {
      sky::PlayerRecord &record = recordFromPeer(event.peer);
      appLog("Client disconnected, PID " + std::to_string(record.pid));
      broadcastToClientsExcept(
          record.pid,
          sky::prot::ServerNotifyDisconnection("no reason given", record.pid));
      arena.disconnectPlayer(record);
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
