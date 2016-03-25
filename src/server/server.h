#ifndef SOLEMNSKY_SERVER_H
#define SOLEMNSKY_SERVER_H

#include "sky/sky.h"
#include "sky/arena.h"
#include "util/telegraph.h"
#include "sky/protocol.h"
#include "sky/event.h"
#include "sky/arena.h"
#include <iostream>

/**
 * Arena render
 */
class ServerLogger: sky::Subsystem {
 protected:
  void registerPlayer(sky::Player &player) override;
  void unregisterPlayer(sky::Player &player) override;

 public:
  ServerLogger(sky::Arena *const arena);
};

class Server {
 private:
  tg::UsageFlag flag; // for enet global state

  double uptime;

  sky::Arena arena;
  ServerLogger logger;

  /**
   * Connection state.
   */
  tg::Host host;
  tg::Telegraph<sky::ClientPacket> telegraph;
  std::vector<ENetPeer *> connectingPeers;

  /**
   * Events.
   */
  void logEvent(const ServerEvent &event);
  void logArenaEvent(const ArenaEvent &event);

  /**
   * Network logic.
   */
  void sendToClients(const sky::ServerPacket &packet);
  void sendToClientsExcept(const PID pid,
                           const sky::ServerPacket &packet);
  void sendToClient(ENetPeer *const client,
                    const sky::ServerPacket &packet);
  sky::Player *playerFromPeer(ENetPeer *peer) const;
  void processPacket(ENetPeer *client, const sky::ClientPacket &packet);

 public:
  Server(const Port port, const sky::ArenaInitializer &initializer);

  void tick(float delta);
  bool running;
};

int main();

#endif //SOLEMNSKY_SERVER_H
