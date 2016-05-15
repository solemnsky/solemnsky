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

void VanillaServer::registerPlayer(sky::Player &player) {

}

void VanillaServer::unregisterPlayer(sky::Player &player) {

}

void VanillaServer::onTick(const float delta) {

}

void VanillaServer::onPacket(ENetPeer *const client,
                             sky::Player &player,
                             const sky::ClientPacket &packet) {
  if (packet.type == sky::ClientPacket::Type::RCon) {
    if (!player.isAdmin()) {
      if (packet.stringData.get() == "auth password") {
        sky::PlayerDelta delta = player.zeroDelta();
        delta.admin = true;
        shared.applyAndSendDelta(sky::ArenaDelta::Delta(player.pid, delta));
        shared.rconResponse(client, "you're authenticated");
      }
    } else {
      // TODO: uniform command parsing
      if (packet.stringData.get() == "start") {
        shared.applyAndSendDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
        return;
      }

      if (packet.stringData.get() == "stop") {
        shared.applyAndSendDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Lobby));
        return;
      }

      if (packet.stringData.get() == "restart") {
        sky.restart();
        return;
      }

      if (packet.stringData.get() == "map1") {
        shared.applyAndSendDelta(sky::ArenaDelta::MapChange("test1"));
        return;
      }

      if (packet.stringData.get() == "map2") {
        shared.applyAndSendDelta(sky::ArenaDelta::MapChange("test2"));
        return;
      }
    }
  }

  if (packet.type == sky::ClientPacket::Type::ReqSpawn) {
    player.spawn({}, {300, 300}, 0);
  }
}

VanillaServer::VanillaServer(ServerShared &telegraphy,
                             sky::Arena &arena, sky::SkyHandle &sky)
    : Server(telegraphy, arena, sky) {
  arena.forPlayers([&](sky::Player &player) { registerPlayer(player); });
}

