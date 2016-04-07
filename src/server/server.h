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
 * Methods that the ServerExec exposes for use by Server and ServerLogger.
 */
struct ServerShared {
 private:
  class ServerExec &exec; // TODO: justify use

 public:
  ServerShared(tg::Host &host, tg::Telegraph<sky::ClientPacket> &telegraph,
               class ServerExec &exec);

  tg::Host &host;
  tg::Telegraph<sky::ClientPacket> &telegraph;

  sky::Player *playerFromPeer(ENetPeer *peer) const;

  // transmission
  void sendToClients(const sky::ServerPacket &packet);
  void sendToClientsExcept(const PID pid,
                           const sky::ServerPacket &packet);
  void sendToClient(ENetPeer *const client,
                    const sky::ServerPacket &packet);

  // logging events
  void logEvent(const ServerEvent &event);
  void logArenaEvent(const sky::ArenaEvent &event);
};

/**
 * The Server abstraction: a sky::Arena subsystem with additional callbacks
 * for networking and access to a ServerShared object.
 */
class Server: public sky::Subsystem {
  friend class ServerExec;
  // callbacks are protected, in the style of sky::Subsystem

 protected:
  ServerShared &shared;
  sky::Sky &sky;

  virtual void onPacket(ENetPeer *const client,
                        sky::Player &player,
                        const sky::ClientPacket &packet) = 0;

 public:
  Server(ServerShared &shared, sky::Arena &arena, sky::Sky &sky);

};

/**
 * Subsystem that just logs arena events through the onEvent callback.
 */
class ServerLogger: public sky::Subsystem {
 private:
  ServerShared &shared;

 protected:
  void registerPlayer(sky::Player &player) override;
  void unregisterPlayer(sky::Player &player) override;
  void onEvent(const sky::ArenaEvent &event) override;

 public:
  ServerLogger(ServerShared &shared, sky::Arena &arena);
};

/**
 * State and logic associated with the execution of a Server.
 * We manage the basics here.
 */
class ServerExec {
  friend class ServerLogger;

 private:
  tg::UsageFlag flag; // for enet global state
  double uptime;

  tg::Host host;
  tg::Telegraph<sky::ClientPacket> telegraph;
  ServerShared shared;

  sky::Arena arena;
  sky::Sky sky;
  std::unique_ptr<Server> server;

  ServerLogger logger;

  void processPacket(ENetPeer *client, const sky::ClientPacket &packet);
  void tick(float delta);

 public:
  ServerExec(const Port port,
             const sky::ArenaInitializer &arena,
             const sky::SkyInitializer &sky,
             std::function<std::unique_ptr<Server>(
                 ServerShared &, sky::Arena &, sky::Sky &)> server);

  void run();

  bool running;
};
