#include "vanilla.h"

void VanillaServer::registerPlayer(sky::Player &player) {
  player.data.push_back(nullptr);
}

void VanillaServer::unregisterPlayer(sky::Player &player) {

}

void VanillaServer::onTick(const float delta) {

}

void VanillaServer::onPacket(ENetPeer *const client,
                             sky::Player &player,
                             const sky::ClientPacket &packet) {
  if (packet.type == sky::ClientPacket::Type::RCon) {
    if (!player.admin) {
      if (packet.stringData.get() == "auth password") {
        sky::PlayerDelta delta = player.zeroDelta();
        delta.admin = true;
        sky::ArenaDelta adminDelta = sky::ArenaDelta::Delta(player.pid, delta);
        arena.applyDelta(adminDelta);
        shared.sendToClients(sky::ServerPacket::DeltaArena(adminDelta));

        shared.rconResponse(client, "you're authenticated");
      }
    }

    if (player.admin) {
      // TODO: uniform command parsing
      if (packet.stringData.get() == "start") {
        sky::ArenaDelta delta = sky::ArenaDelta::Mode(sky::ArenaMode::Game);
        arena.applyDelta(delta);
        shared.sendToClients(sky::ServerPacket::DeltaArena(delta));
      }

      if (packet.stringData.get() == "stop") {
        sky::ArenaDelta delta = sky::ArenaDelta::Mode(sky::ArenaMode::Lobby);
        arena.applyDelta(delta);
        shared.sendToClients(sky::ServerPacket::DeltaArena(delta));
      }

      if (packet.stringData.get() == "restart") {
        sky.restart();
      }
    }
  }
}

VanillaServer::VanillaServer(ServerShared &telegraphy,
                             sky::Arena &arena, sky::Sky &sky)
    : Server(telegraphy, arena, sky) {
  arena.forPlayers([&](sky::Player &player) { registerPlayer(player); });
}

