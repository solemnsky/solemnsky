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
#include "servershared.hpp"

/**
 * ServerShared.
 */

ServerShared::ServerShared(
    tg::Host &host, tg::Telegraph<sky::ClientPacket> &telegraph,
    const sky::ArenaInit &arenaInit) :
    arena(arenaInit), // initialize engine state
    skyHandle(arena, {}),
    scoreboard(arena, {}),

    host(host),
    telegraph(telegraph) { }

sky::Player *ServerShared::playerFromPeer(ENetPeer *peer) const {
  if (peer->data) return (sky::Player *) peer->data;
  else return nullptr;
}

void ServerShared::registerArenaDelta(const sky::ArenaDelta &arenaDelta) {
  arena.applyDelta(arenaDelta);
  sendToClients(sky::ServerPacket::DeltaArena(arenaDelta));
}

void ServerShared::registerGameStart() {
  skyHandle.start();
  registerArenaDelta(sky::ArenaDelta::ResetEnvLoad());
}

void ServerShared::registerGameEnd() {
  skyHandle.stop();
}

void ServerShared::sendToClients(const sky::ServerPacket &packet) {
  telegraph.transmit(
      host,
      [&](
          std::function<void(ENetPeer *const)> transmit) {
        for (auto const peer : host.getPeers()) { transmit(peer); }
      }, packet);
}

void ServerShared::sendToClientsExcept(const PID pid,
                                       const sky::ServerPacket &packet) {
  telegraph.transmit(
      host,
      [&](
          std::function<void(ENetPeer *const)> transmit) {
        for (auto const peer : host.getPeers()) {
          if (sky::Player *player = playerFromPeer(peer)) {
            if (player->pid != pid) transmit(peer);
          }
        }
      }, packet);
}

void ServerShared::sendToClient(ENetPeer *const client,
                                const sky::ServerPacket &packet) {
  telegraph.transmit(host, client, packet);
}

void ServerShared::rconResponse(ENetPeer *const client,
                                const std::string &response) {
  sendToClient(client, sky::ServerPacket::RCon(response));
  logEvent(ServerEvent::RConOut(response));
}

void ServerShared::logEvent(const ServerEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Server);
}

void ServerShared::logArenaEvent(const sky::ArenaEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Engine);
}

