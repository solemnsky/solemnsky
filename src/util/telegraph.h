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
/**
 * Network utilities.
 */
#pragma once
#include "enet/enet.h"
#include "util/types.h"
#include <sstream>
#include <cereal/archives/binary.hpp>
#include "util/methods.h"
#include "printer.h"
#include <boost/range/iterator_range_core.hpp>

namespace tg {

/**
 * ENet functionality can be used iff one of these is alive.
 * We need this because of enet's global state initialization /
 * deinitialization.
 */
struct UsageFlag {
  UsageFlag();
  ~UsageFlag();
};

/**
 * Host type: client or server.
 */
enum class HostType { Client, Server };

class Host {
 private:
  ENetHost *host;
  ENetEvent event;

  /**
   * Keeping track of peers.
   */
  void registerPeer(ENetPeer *peer);
  void unregisterPeer(ENetPeer *peer);

 public:
  Host(const HostType type,
       const Port port = 0);
  ~Host();

  // uncopyable + unassignable
  Host(const Host &) = delete;
  Host &operator=(const Host &) = delete;

  /**
   * API.
   */
  std::vector<ENetPeer *> peers;
  ENetPeer *connect(const std::string &address, const Port port);
  void disconnect(ENetPeer *);
  void transmit(ENetPeer *const peer,
                unsigned char *data, size_t size,
                const ENetPacketFlag flag);
  ENetEvent poll();
};

/**
 * Helps us send / receive data through a tg::Host.
 */
template<typename ReceiveType>
class Telegraph {
 private:
  ReceiveType receiveBuffer;

 public:
  Telegraph() { }

  // TODO: use the stringstream less stupidly?

  template<typename TransmitType>
  std::string outputToString(const TransmitType &x) {
    std::stringstream outputStream;
    cereal::BinaryOutputArchive output(outputStream);
    output(x);
    return outputStream.str();
  }

  /**
   * Transmit a packet to one peer.
   */
  template<typename TransmitType>
  void transmit(
      Host &host, ENetPeer *const peer,
      const TransmitType &value,
      ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE) {
    std::string data = outputToString(value);
    host.transmit(peer, (unsigned char *) data.c_str(), data.size(), flag);
  }

  /**
   * Transmit same packet to an iterator range of peers.
   */
  template<typename TransmitType>
  void transmit(
      Host &host,
      std::function<void(std::function<void(ENetPeer *const)>)> callPeers,
      const TransmitType &value,
      ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE) {
    std::string data = outputToString(value);
    callPeers([&](ENetPeer *const peer) {
      host.transmit(peer, (unsigned char *) data.c_str(), data.size(), flag);
    });
  }

  optional<ReceiveType> receive(const ENetPacket *packet) {
    std::string data((const char *) packet->data, packet->dataLength);
    std::stringstream inputStream(data);
    cereal::BinaryInputArchive input(inputStream);

    receiveBuffer = ReceiveType();
    try {
      input(receiveBuffer);
      if (!verifyValue(receiveBuffer)) {
        appLog("Malformed packet: violated invariants!", LogOrigin::Network);
      } else {
        return receiveBuffer;
      }
    } catch (...) {
      appLog("Malformed packet: failed to decode!", LogOrigin::Network);
    }
    return {};
  }
};

std::string printAddress(const ENetAddress &addr);

}
