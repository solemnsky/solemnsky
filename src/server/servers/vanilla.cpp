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
      }

      if (packet.stringData.get() == "stop") {
        shared.applyAndSendDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Lobby));
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

