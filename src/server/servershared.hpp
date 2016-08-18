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
 * Central state / methods, referenced by everything with executive power in the server.
 */
#pragma once
#include "engine/arena.hpp"
#include "util/telegraph.hpp"
#include "server/engine/latencytracker.hpp"
#include "engine/protocol.hpp"
#include "engine/event.hpp"

/**
 * Shared object for the server, holding engine state and network
 * / logging methods.
 */
struct ServerShared {
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

  // Centralized state modification / synchronization.
  void registerArenaDelta(const sky::ArenaDelta &arenaDelta);
  void registerGameStart();
  void registerGameEnd();

  // Transmission.
  void sendToClients(const sky::ServerPacket &packet);
  void sendToClientsExcept(const PID pid,
                           const sky::ServerPacket &packet);
  void sendToClient(ENetPeer *const client,
                    const sky::ServerPacket &packet);

  void rconResponse(ENetPeer *const client, const std::string &response);

  // Logging.
  void logEvent(const ServerEvent &event);
  void logArenaEvent(const sky::ArenaEvent &event);

};

