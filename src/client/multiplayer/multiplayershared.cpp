#include "multiplayershared.h"

/**
 * MultiplayerShared.
 */
MultiplayerShared::MultiplayerShared(
    const std::string &serverHostname,
    const unsigned short serverPort) :
    host(tg::HostType::Client),
    server(nullptr),
    telegraph(sky::ServerPacketPack(), sky::ClientPacketPack()),
    pingCooldown(5),
    triedConnection(false),
    disconnecting(false),
    disconnectTimeout(1) {
  host.connect(serverHostname, serverPort);
}

void MultiplayerShared::transmitPacket(const sky::ClientPacket &packet) {
  if (server) telegraph.transmit(host, server, packet);
}

/**
 * MultiplayerMode.
 */

MultiplayerMode::MultiplayerMode(MultiplayerShared &shared) :
    shared(shared) { }

