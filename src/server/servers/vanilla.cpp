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
#include "vanilla.h"

void VanillaServer::tickGame(const TimeDiff delta, sky::Sky &sky) {
  arena.forPlayers([&](sky::Player &player) {
    auto &participation = sky.getParticipation(player);
    if (participation.isSpawned()) {
      if (participation.getControls().getState<sky::Action::Primary>()) {
        if (participation.plane->requestDiscreteEnergy(0.5)) {
          appLog("pewpew");
        }
      }
    }
  });
}

void VanillaServer::onTick(const TimeDiff delta) {
  if (shared.skyHandle.isActive())
    tickGame(delta, shared.skyHandle.sky.get());

}

void VanillaServer::onPacket(ENetPeer *const client,
                             sky::Player &player,
                             const sky::ClientPacket &packet) {
  if (packet.type == sky::ClientPacket::Type::RCon) {
    if (!player.isAdmin()) {
      if (packet.stringData.get() == "auth") {
        sky::PlayerDelta delta{player};
        delta.admin = true;
        shared.registerArenaDelta(sky::ArenaDelta::Delta(player.pid, delta));
        shared.rconResponse(client, "you're authenticated");
      }
    } else {
      // TODO: uniform command parsing
      if (packet.stringData.get() == "start") {
        skyHandle.start();
        shared.registerArenaDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
        return;
      }

      if (packet.stringData.get() == "stop") {
        skyHandle.stop();
        shared.registerArenaDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Lobby));
        return;
      }

      if (packet.stringData.get() == "restart") {
        skyHandle.start();
        return;
      }

      if (packet.stringData.get() == "map1") {
        shared.registerArenaDelta(sky::ArenaDelta::MapChange("test1"));
        skyHandle.start();
        return;
      }

      if (packet.stringData.get() == "map2") {
        shared.registerArenaDelta(sky::ArenaDelta::MapChange("test2"));
        skyHandle.start();
        return;
      }
    }
  }

  if (packet.type == sky::ClientPacket::Type::ReqTeam) {
    if (arena.getMode() == sky::ArenaMode::Lobby) {
      appLog("changing team");
      sky::PlayerDelta delta{player};
      delta.team = packet.team.get();
      shared.registerArenaDelta(sky::ArenaDelta::Delta(player.pid, delta));
      assert(player.getTeam() == packet.team.get());
    }
  }

  if (packet.type == sky::ClientPacket::Type::ReqSpawn) {
    if (player.getTeam() != 0) {
      player.spawn({}, {300, 300}, 0);
    }
  }
}

VanillaServer::VanillaServer(ServerShared &shared) :
    Server(shared) { }

