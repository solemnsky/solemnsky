#include "server.h"
#include "util/methods.h"

PlayerClient::PlayerClient(const sf::IpAddress &address,
                           const unsigned short port) :
    address(address), port(port) { }

Server::Server() :
    telegraph(4242, sky::pk::serverPacketPack, sky::pk::clientPacketPack),
    running(true) { }

void Server::tick(float delta) {
  using namespace sky::prot;

  uptime += delta;
  telegraph.receive([&](tg::Reception<ClientPacket> &&reception) {
    switch (reception.value.type) {
      case ClientPacket::Type::Ping: {
        appLog(LogType::Info, "ping request recieved.");
        appLog(LogType::Info, "responding with pong.");
        telegraph.transmit(ServerPong(), reception.address, 4243);
        break;
      }
      case ClientPacket::Type::MotD: {
        appLog(LogType::Info, "MotD request received.");
        appLog(LogType::Info, "responding with MotD.");
        telegraph.transmit(
            ServerMotD(
                std::string("Wheeeee I'm in a UDP packet look at me!")),
            reception.address, 4243);
        break;
      }
      case ClientPacket::Type::Chat: {
        appLog(LogType::Info, "chat packet received.");
        appLog(LogType::Info, ">>" + *reception.value.stringData);
        telegraph.transmit(
            ServerMessage(std::string(*reception.value.stringData)),
            reception.address, 4243
        );
        break;
      }
    }
  });
}

int main() {
  Server server;
  sf::Clock clock;

  while (server.running) {
    server.tick(clock.restart().asSeconds());
    sf::sleep(sf::milliseconds(16));
  }
}

