#ifndef SOLEMNSKY_SERVER_H
#define SOLEMNSKY_SERVER_H

#include "sky/sky.h"
#include "sky/arena.h"
#include "util/telegraph.h"
#include "sky/protocol.h"
#include "sky/arena.h"
#include <iostream>

class Server {
 private:
  tg::UsageFlag flag; // for enet global state

  double uptime;
  sky::Arena arena;

  /**
   * Connection stuff.
   */
  tg::Host host;
  tg::Telegraph<sky::ClientPacket> telegraph;
  ENetEvent event;
  std::vector<ENetPeer *> connectingPeers;

  /**
   * Network logic.
   */
  void broadcastToClients(const sky::ServerPacket &packet);
  void broadcastToClientsExcept(const sky::PID pid,
                                const sky::ServerPacket &packet);
  void transmitToClient(ENetPeer *const client,
                        const sky::ServerPacket &packet);
  sky::Player *playerFromPeer(ENetPeer *peer) const;
  void processPacket(ENetPeer *client, const sky::ClientPacket &packet);

 public:
  Server(const unsigned short port);

  void tick(float delta);
  bool running;
};

int main();

#endif //SOLEMNSKY_SERVER_H
