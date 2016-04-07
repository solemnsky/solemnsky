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
    if (packet.stringData.get() == "auth password") {
      telegraphy.sendToClients(sky::ServerPacket::Broadcast(
          "our secure authentication algorithm has succeeded"));
      telegraphy.sendToClient(client, sky::ServerPacket::RCon(
          "beep boop, you're authorized"
      ));
    }

    if (packet.stringData.get() == "start") {
      sky::ArenaDelta delta = sky::ArenaDelta::Mode(sky::ArenaMode::Game);
      arena.applyDelta(delta);
      telegraphy.sendToClients(sky::ServerPacket::DeltaArena(delta));
    }

    if (packet.stringData.get() == "stop") {
      sky::ArenaDelta delta = sky::ArenaDelta::Mode(sky::ArenaMode::Lobby);
      arena.applyDelta(delta);
      telegraphy.sendToClients(sky::ServerPacket::DeltaArena(delta));
    }
  }
}

VanillaServer::VanillaServer(ServerTelegraphy &telegraphy,
                             sky::Arena &arena, sky::Sky &sky)
    : Server(telegraphy, arena, sky) {
  arena.forPlayers([&](sky::Player &player) { registerPlayer(player); });
}

