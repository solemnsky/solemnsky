#include "telegraph.h"
#include "util/methods.h"

namespace tg {

/**
 * UsageFlag.
 */
using detail::enetUsageCount;

UsageFlag::UsageFlag() {
  enetUsageCount++;
  appLog("Initializing ENet...", LogOrigin::Network);
  if (enetUsageCount == 1) {
    if (enet_initialize() != 0)
      appErrorRuntime("Failure on ENet initialization!");
  }
}

UsageFlag::~UsageFlag() {
  enetUsageCount--;
  appLog("Deinitializing ENet...", LogOrigin::Network);
  if (enetUsageCount == 0) {
    enet_deinitialize();
  }
}

/**
 * Host.
 */
Host::Host(const HostType type, const unsigned short port) {
  switch (type) {
    case HostType::Server: {
      ENetAddress address;
      address.host = ENET_HOST_ANY;
      address.port = port;
      // no upstream / downstream bandwidth limits
      host = enet_host_create(&address, 32, 1, 0, 0);
    }
    case HostType::Client: {
      // sensible upstream / downstream bandwidth limits
      host = enet_host_create(nullptr, 1, 1, 57600 / 8, 14400 / 8);
    }
  }

  if (host == NULL) appErrorRuntime("Failed to initialize ENet host!");
}

Host::~Host() {
  enet_host_destroy(host);
}

ENetPeer *Host::connect(const std::string &address, const unsigned short port) {
  ENetAddress eaddress;
  enet_address_set_host(&eaddress, address.c_str());
  eaddress.port = port;
  return enet_host_connect(host, &eaddress, 1, 0);
}

void Host::disconnect(ENetPeer *peer) {
  enet_peer_disconnect(peer, 0);
}

void Host::transmit(ENetPeer *const peer,
                    unsigned char *data,
                    size_t size,
                    const ENetPacketFlag flag) {
  ENetPacket *packet = enet_packet_create(data, size, flag);
  enet_peer_send(peer, 0, packet);
}

ENetEvent Host::poll() {
  enet_host_service(host, &event, 0);
  return event;
}

}
