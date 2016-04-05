/**
 * Common abstraction for our multiplayer servers.
 */
#pragma once
#include "sky/sky.h"
#include "sky/arena.h"
#include "util/telegraph.h"
#include "sky/protocol.h"
#include "sky/event.h"
#include "sky/arena.h"
#include "sky/event.h"
#include <iostream>

/**
 * Telegraphy state and utilities for use by the Server and the ServerExec.
 */
struct ServerTelegraphy {
  ServerTelegraphy(tg::Host &host);

  tg::Host &host;
  tg::Telegraph<sky::ClientPacket> telegraph;

  sky::Player *playerFromPeer(ENetPeer *peer) const;
  void sendToClients(const sky::ServerPacket &packet);
  void sendToClientsExcept(const PID pid,
                           const sky::ServerPacket &packet);
  void sendToClient(ENetPeer *const client,
                    const sky::ServerPacket &packet);
};

/**
 * The Server abstraction: a sky::Arena subsystem with additional callbacks
 * for networking and access to a ServerTelegraphy object.
 */
class Server: public sky::Subsystem {
  friend class ServerExec;
 protected:
  ServerTelegraphy &telegraphy;
  sky::Sky &sky;

  virtual void onPacket(ENetPeer &client,
                        sky::Player &player,
                        const sky::ClientPacket &packet) = 0;

 public:
  Server(ServerTelegraphy &telegraphy, sky::Arena &arena, sky::Sky &sky);

};

/**
 * State and logic associated with the execution of a Server.
 * We manage the basics here.
 */
class ServerExec {
 private:
  tg::UsageFlag flag; // for enet global state

  double uptime;

  tg::Host host;
  ServerTelegraphy telegraphy;
  sky::Arena arena;
  sky::Sky sky;
  std::unique_ptr<Server> server;

  void logEvent(const ServerEvent &event);
  void logArenaEvent(const ArenaEvent &event);

  void processPacket(ENetPeer *client, const sky::ClientPacket &packet);
  void tick(float delta);

 public:
  ServerExec(const Port port, const sky::ArenaInitializer &initializer,
             std::function<std::unique_ptr<Server>(
                 ServerTelegraphy &, sky::Arena &, sky::Sky &)> f);

  void run();

  bool running;
};
