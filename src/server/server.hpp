/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * Common abstraction for our multiplayer servers.
 */
#pragma once
#include <iostream>
#include "engine/arena.hpp"
#include "util/telegraph.hpp"
#include "latencytracker.hpp"
#include "engine/protocol.hpp"
#include "engine/event.hpp"

/**
 * Shared object for the server, holding engine state and network
 * / logging methods.
 */
struct ServerShared {
 public:
  ServerShared(tg::Host &host, tg::Telegraph<sky::ClientPacket> &telegraph,
               const sky::ArenaInit &arenaInit);

  // Engine state.
  sky::Arena arena;
  sky::SkyHandle skyHandle;
  sky::Scoreboard scoreboard;

  // Network state.
  tg::Host &host;
  tg::Telegraph<sky::ClientPacket> &telegraph;
  sky::Player *playerFromPeer(ENetPeer *peer) const;

  // Registering arena deltas.
  void registerArenaDelta(const sky::ArenaDelta &arenaDelta);

  // Transmission.
  void sendToClients(const sky::ServerPacket &packet,
                     const bool guaranteeOrder = true);
  void sendToClientsExcept(const PID pid,
                           const sky::ServerPacket &packet,
                           const bool guaranteeOrder = true);
  void sendToClient(ENetPeer *const client,
                    const sky::ServerPacket &packet,
                    const bool guaranteeOrder = true);
  void rconResponse(ENetPeer *const client, const std::string &response);

  // Logging.
  void logEvent(const ServerEvent &event);
  void logArenaEvent(const sky::ArenaEvent &event);

};

/**
 * Type-erasure for Server, representing the uniform API.
 */
class ServerListener: public sky::SubsystemListener {
  friend class ServerExec;
 protected:
  virtual void onPacket(ENetPeer *const client,
                        sky::Player &player,
                        const sky::ClientPacket &packet) = 0;

};

/**
 * The Server abstraction: a sky::Arena subsystem with additional callbacks
 * for networking and access to a ServerShared object.
 */
template<typename PlayerData>
class Server: public ServerListener, public sky::Subsystem<PlayerData> {
 protected:
  ServerShared &shared;
  sky::SkyHandle &skyHandle;
  sky::Scoreboard &scoreboard;

 public:
  Server(ServerShared &shared) :
      sky::Subsystem<PlayerData>(shared.arena),
      shared(shared),
      skyHandle(shared.skyHandle),
      scoreboard(shared.scoreboard) { }

};

/**
 * Subsystem that just logs arena events through the onEvent callback.
 */
class ServerLogger: public sky::ArenaLogger {
 private:
  ServerShared &shared;

 protected:
  void onEvent(const sky::ArenaEvent &event) override;

 public:
  ServerLogger(ServerShared &shared, sky::Arena &arena);

};

/**
 * State and logic associated with the execution of a Server.
 * We manage the basics here.
 */
class ServerExec {
  friend struct ServerShared;

 private:
  tg::UsageFlag flag; // for enet global state

  // Networking state.
  tg::Host host;
  tg::Telegraph<sky::ClientPacket> telegraph;
  ServerShared shared;

  // Packet scheduling.
  Cooldown skyDeltaTimer,
      scoreDeltaTimer,
      pingTimer,
      latencyUpdateTimer;

  // Attached server.
  std::unique_ptr<ServerListener> server;

  // Subsystems.
  ServerLogger logger;
  LatencyTracker latencyTracker;

  // Server loop subroutines.
  void processPacket(ENetPeer *client, const sky::ClientPacket &packet);
  // (returns true when the queue has been exhausted)
  bool poll();
  void tick(const TimeDiff delta);

 public:
  ServerExec(const Port port,
             const sky::ArenaInit &arenaInit,
             std::function<std::unique_ptr<ServerListener>(
                 ServerShared &)> mkServer);

  void run();

  bool running;
};
