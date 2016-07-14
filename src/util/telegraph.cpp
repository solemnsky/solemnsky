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
#include "telegraph.hpp"
#include "util/methods.hpp"

namespace tg {

/**
 * UsageFlag.
 */
static int enetUsageCount = 0;

UsageFlag::UsageFlag() {
  enetUsageCount++;
  if (enetUsageCount == 1) {
    if (enet_initialize() != 0)
      throw std::runtime_error("Failure on ENet initialization!");
    appLog("Initialized ENet!", LogOrigin::Network);
  }
}

UsageFlag::~UsageFlag() {
  enetUsageCount--;
  if (enetUsageCount == 0) {
    enet_deinitialize();
    appLog("Deinitialized ENet.", LogOrigin::Network);
  }
}

/**
 * Host.
 */

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

void Host::sampleBandwidth() {
  lastIncomingBandwidth = float(host->totalReceivedData) / 1000.0f;
  lastOutgoingBandwidth = float(host->totalSentData) / 1000.0f;
  host->totalReceivedData = 0;
  host->totalSentData = 0;
}

Host::Host(const HostType type, const Port port) :
    host(nullptr),
    bandwidthSampler(1) {
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
      throw std::runtime_error("Failed to create ENet host: ENet global state "
                                   "not initialized!");
    }
    throw std::runtime_error("Failed to create ENet host!");
  }

  sampleBandwidth();
}

Host::~Host() {
  enet_host_destroy(host);
}

const std::vector<ENetPeer *> &Host::getPeers() const {
  return peers;
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

void Host::tick(const TimeDiff delta) {
  if (bandwidthSampler.cool(delta)) {
    sampleBandwidth();
    bandwidthSampler.reset();
  }
}

Kbps Host::incomingBandwidth() const {
  return lastIncomingBandwidth;
}

Kbps Host::outgoingBandwidth() const {
  return lastOutgoingBandwidth;
}

std::string printAddress(const ENetAddress &addr) {
  union {
    ENetAddress addr;
    unsigned char ch[4];
  } convenient;
  convenient.addr = addr;

  return std::to_string(int(convenient.ch[0])) + "."
       + std::to_string(int(convenient.ch[1])) + "."
       + std::to_string(int(convenient.ch[2])) + "."
       + std::to_string(int(convenient.ch[3])) + ":"
       + std::to_string(int(addr.port));
}

}
