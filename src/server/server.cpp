#include "server.h"
#include "util/methods.h"

PlayerClient::PlayerClient(const sf::IpAddress &address,
                           const unsigned short port) :
    address(address), port(port) { }

Server::Server() :
    telegraph(4242, sky::pk::serverPacketPack, sky::pk::clientPacketPack),
    running(true) { }

/**
 * Helpers.
 */
sky::PID Server::getFreePID() const {
  sky::PID maxId = 0;
  for (auto &pair : arena.players) maxId = std::max(pair.first, maxId);
  return maxId;
}

optional<sky::PID> Server::pidFromIP(const sf::IpAddress &address) const {
  for (auto &pair : clients)
    if (pair.second.address == address) return pair.first;
  return {};
}

void Server::sendToClient(const sky::prot::ServerPacket &packet,
                          const sky::PID pid) {
  if (clients.find(pid) != clients.end()) {
    const PlayerClient &client = clients.at(pid);
    telegraph.transmit(packet, client.address, client.port);
  }
}

void Server::broadcastToClients(const sky::prot::ServerPacket &packet) {
  for (auto &pair : clients)
    telegraph.transmit(packet, pair.second.address, pair.second.port);
}

/**
 * Processing.
 */
void Server::tick(float delta) {
  if (sky) sky->tick(delta);

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
      case ClientPacket::Type::ReqConnection: {
        sky::PID pid = getFreePID();
        clients.emplace(std::pair<sky::PID, PlayerClient>(
            pid, PlayerClient(reception.address, *reception.value.port)));
        sendToClient(ServerAcceptConnection(arena, pid), pid);
        appLog(LogType::Info, "connection granted to client");
        break;
      }
      case ClientPacket::Type::ReqNickChange: {
        break;
      }
      case ClientPacket::Type::Chat: {
        appLog(LogType::Info, "chat packet received.");
        appLog(LogType::Info, ">>" + *reception.value.stringData);
        broadcastToClients(
            ServerMessage(std::string(*reception.value.stringData)));
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
