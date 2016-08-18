/**
 * Central state / methods, referenced by everything with executive power in the server.
 */
#pragma once
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

