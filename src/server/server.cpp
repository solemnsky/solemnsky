#include "server.h"
#include "util/methods.h"

/**
 * PlayerClient.
 */
PlayerClient::PlayerClient(const sf::IpAddress &address,
                           const unsigned short port) :
    address(address), port(port) { }

/**
 * Server.
 */

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

optional<sky::PID> Server::clientFromIP(
    const sf::IpAddress &address) const {
  for (const auto &pair : clients)
    if (pair.second.address == address) return pair.first;
  return {};
}

/**
 * Transmission synonyms.
 */
void Server::sendToClient(const sky::prot::ServerPacket &packet,
                          const PlayerClient &client) {
  telegraph.transmit(packet, client.address, client.port);
}

void Server::sendToAll(const sky::prot::ServerPacket &packet) {
  for (const auto &pair : clients)
    sendToClient(packet, pair.second);
}

/**
 * Subroutines
 */
void Server::processFromClient(sky::PID pid,
                               const sky::prot::ClientPacket &packet) {
  using namespace sky::prot;

  PlayerClient &client = clients.at(pid);
  client.lastPing = uptime;

  switch (packet.type) {
    case ClientPacket::Type::Ping: {
      sendToClient(ServerPong(), client);
      break;
    }
    case ClientPacket::Type::ReqNickChange: {
      arena.players.at(pid).nickname = *packet.stringData;
      sendToAll(ServerAssignNick(pid, std::string(*packet.stringData)));
      break;
    }
    case ClientPacket::Type::Chat: {
      sendToAll(ServerMessage(std::string(*packet.stringData)));
      break;
    }
    default:
      break;
  }
}

void Server::processConnection(
    tg::Reception<sky::prot::ClientPacket> &&reception) {
  using namespace sky::prot;

  switch (reception.value.type) {
    case ClientPacket::Type::ReqConnection: {
      sky::PID pid = getFreePID();
      clients.emplace(
          pid, PlayerClient(reception.address, *reception.value.port));
      sendToClient(ServerAcceptConnection(arena, pid), clients.at(pid));
      break;
    };
    default:
      break;
  }
}

void Server::tick(float delta) {
  if (sky) sky->tick(delta);

  uptime += delta;
  telegraph.receive([&](tg::Reception<sky::prot::ClientPacket> &&reception) {
    if (optional<sky::PID> pid = clientFromIP(reception.address)) {
      processFromClient(*pid, reception.value);
    } else {
      processConnection(std::move(reception));
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
