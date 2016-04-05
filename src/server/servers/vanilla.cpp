#include "vanilla.h"


void VanillaServer::registerPlayer(sky::Player &player) {
  player.data.push_back(nullptr);
}

void VanillaServer::unregisterPlayer(sky::Player &player) {

}

void VanillaServer::onTick(const float delta) {

}

void virtual VanillaServer::onPacket(ENetPeer &client,
                                     sky::Player &player,
                                     const sky::ClientPacket &packet) {
  if (packet.type == sky::ClientPacket::Type::RCon) {
    if (packet.stringData.get() == "some command") {
      telegraphy.sendToClients(sky::ServerPacket::Message(
          sky::ServerMessage::Broadcast("some effect")
      ));
    }
  }
}

VanillaServer::VanillaServer(ServerTelegraphy &telegraphy,
                             sky::Arena &arena, sky::Sky &sky)
    : Server(telegraphy, arena, sky) {
  arena.forPlayers([&](sky::Player &player) { registerPlayer(player); });
}

