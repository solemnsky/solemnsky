#include "telegraph.h"
#include "util/methods.h"

namespace tg {

/**
 * UsageFlag.
 */
static int enetUsageCount = 0;

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
Host::Host(const HostType type, const Port port) :
    host(nullptr) {
  switch (type) {
    case HostType::Server: {
      ENetAddress address;
      address.host = ENET_HOST_ANY;
      address.port = port;
      // no upstream / downstream bandwidth limits
      host = enet_host_create(&address, 32, 1, 0, 0);
      break;
    }
    case HostType::Client: {
      // sensible upstream / downstream bandwidth limits
      host = enet_host_create(nullptr, 2, 1, 57600 / 8, 14400 / 8);
      break;
    }
  }

  if (host == nullptr) {
    if (enetUsageCount == 0) {
      appErrorRuntime("Failed to create ENet host: ENet global state "
                          "not initialized!");
    }
    appErrorRuntime("Failed to create ENet host!");
  }
}

Host::~Host() {
  enet_host_destroy(host);
}

void Host::registerPeer(ENetPeer *peer) {
  auto found = std::find(peers.begin(), peers.end(), peer);
  if (found != peers.end()) {
    *found = peer;
  } else {
    peers.push_back(peer);
  }
}

void Host::unregisterPeer(ENetPeer *peer) {
  auto found = std::find(peers.begin(), peers.end(), peer);
  if (found != peers.end()) {
    peers.erase(found);
  }
}

ENetPeer *Host::connect(const std::string &address, const Port port) {
  ENetAddress eaddress;
  enet_address_set_host(&eaddress, address.c_str());
  eaddress.port = port;
  ENetPeer *peer = enet_host_connect(host, &eaddress, 1, 0);
  return peer;
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

  if (event.type == ENET_EVENT_TYPE_CONNECT)
    registerPeer(event.peer);
  else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
    unregisterPeer(event.peer);

  return event;
}

std::string printAddress(const ENetAddress &addr) {
  // TODO
  return "<address>";
}

}
