#ifndef SOLEMNSKY_SERVER_H
#define SOLEMNSKY_SERVER_H

#include "sky/sky.h"
#include "sky/arena.h"
#include "telegraph/telegraph.h"
#include "sky/delta.h"
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
  tg::Telegraph<sky::prot::ClientPacket, sky::prot::ServerPacket> telegraph;
  ENetEvent event;

  /**
   * Packet processing subroutines.
   */
  void processPacket(ENetPeer *client, const sky::prot::ClientPacket &packet);

 public:
  Server(const unsigned short port);

  void tick(float delta);

  bool running;
};

int main();

#endif //SOLEMNSKY_SERVER_H
