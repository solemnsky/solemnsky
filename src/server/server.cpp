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

void Server::processPacket(ENetPeer *client,
                           const sky::prot::ClientPacket &packet) {
  sky::PlayerRecord &record = *((sky::PlayerRecord *) client->data);

  const std::string &pidString = std::to_string(record.pid);

  switch (packet.type) {
    case sky::prot::ClientPacket::Type::Ping: {
      appLog("received ping from " + record.pid, LogOrigin::Server);
      telegraph.transmit(
          host, client,
          sky::prot::ServerPong());
      break;
    }
    case sky::prot::ClientPacket::Type::ReqConnection: {
      appLog(
          "client " + pidString + " joining server with nick "
              + *packet.stringData,
          LogOrigin::Server);
      record.nickname = *packet.stringData;
      break;
    }
    case sky::prot::ClientPacket::Type::ReqNick: {
      appLog(
          "client " + pidString + " changing nick to "
              + *packet.stringData,
          LogOrigin::Server);
      record.nickname = *packet.stringData;
      break;
    }
    case sky::prot::ClientPacket::Type::Chat: {
      appLog(
          "client " + pidString + " says: " + *packet
              .stringData,
          LogOrigin::Server);
      break;
    }
  }
}

void Server::tick(float delta) {
  event = host.poll();
  switch (event.type) {
    case ENET_EVENT_TYPE_NONE:
      break;
    case ENET_EVENT_TYPE_CONNECT: {
      sky::prot::ClientPacket packet = telegraph.receive(event.packet);
      sky::PlayerRecord &record = arena.connectPlayer();
      event.peer->data = &record;
      break;
    }
    case ENET_EVENT_TYPE_DISCONNECT: {
      arena.disconnectPlayer(*((sky::PlayerRecord *) event.peer->data));
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
