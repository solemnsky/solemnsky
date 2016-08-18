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
#include "servershared.hpp"
#include "server/engine/skyinputcache.hpp"

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
 * ArenaLogger to log Arena events to console.
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
 * Basic executor for a server. Implements the server-side multiplayer protocol.
 */
class ServerExec {
  friend struct ServerShared;

 private:
  tg::UsageFlag flag; // for enet global state

  // Networking state.
  tg::Host host;
  tg::Telegraph<sky::ClientPacket> telegraph;
  ServerShared shared;
  SkyInputManager inputManager;

  // Packet scheduling.
  Scheduler skyDeltaSchedule,
      scoreDeltaSchedule,
      pingSchedule,
      latencyUpdateSchedule;

  // Server, to which all more complex logic is delegated.
  std::unique_ptr<ServerListener> server;

  // Subsystems.
  ServerLogger logger;
  LatencyTracker latencyTracker;

  // Application loop subroutines.
  void processPacket(ENetPeer *client, const sky::ClientPacket &packet);
  bool poll(); // (returns true when the queue has been exhausted)
  void tick(const TimeDiff delta);

 public:
  ServerExec(const Port port,
             const sky::ArenaInit &arenaInit,
             std::function<std::unique_ptr<ServerListener>(
                 ServerShared &)> mkServer);

  void run();

  bool running;
};
