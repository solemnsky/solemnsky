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
      // do something
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
      if (packet.stringData.get() == "auth password") {
        sky::PlayerDelta delta = player.zeroDelta();
        delta.admin = true;
        shared.registerArenaDelta(sky::ArenaDelta::Delta(player.pid, delta));
        shared.rconResponse(client, "you're authenticated");
      }
    } else {
      // TODO: uniform command parsing & validation

      // Split the command by whitespace
      std::stringstream tmp(packet.stringData.get());
      std::vector<std::string> command{std::istream_iterator<std::string>{tmp},
                                       std::istream_iterator<std::string>{}};

      if (command[0] == "start") {
        skyHandle.start();
        shared.registerArenaDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
        return;
      }

      if (command[0] == "stop") {
        skyHandle.stop();
        shared.registerArenaDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Lobby));
        return;
      }

      if (command[0] == "restart") {
        skyHandle.start();
        return;
      }

      if (command[0] == "map") {
        if (command.size() < 2){
          shared.rconResponse(client, "Usage: /map <name>");
          return;
        }
        shared.registerArenaDelta(sky::ArenaDelta::MapChange(command[1]));
        skyHandle.start();
        return;
      }
    }
  }

  if (packet.type == sky::ClientPacket::Type::ReqSpawn) {
    player.spawn({}, {300, 300}, 0);
  }
}

VanillaServer::VanillaServer(ServerShared &shared) :
    Server(shared) { }

