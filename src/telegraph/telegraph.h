/**
 * Little helper object for sending / receiving through enetcpp; manages
 * serialization buffers and such.
 */
#ifndef SOLEMNSKY_TELEGRAPH_H
#define SOLEMNSKY_TELEGRAPH_H

#include "enet/enet.h"
#include "util/types.h"
#include "pack.h"
#include "util/methods.h"
#include <boost/range/iterator_range_core.hpp>

namespace tg {

namespace detail {
static int enetUsageCount = 0;
}

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

/**
 * General encapsulation of a ENetHost.
 */
class Host {
 private:
  ENetHost *host;
  ENetEvent event;

 public:
  Host(const HostType type,
       const unsigned short port = 0);
  ~Host();

  // uncopyable + unassignable
  Host(const Host &) = delete;
  Host &operator=(const Host &) = delete;

  /**
   * API.
   */
  ENetPeer *connect(const std::string &address, const unsigned short port);
  boost::iterator_range<ENetPeer *> getPeers();
  void disconnect(ENetPeer *);
  void transmit(ENetPeer *const peer,
                unsigned char *data, size_t size,
                const ENetPacketFlag flag);
  ENetEvent poll();
};

/**
 * Simple helper to hold some boring data and help transmit / receive packets
 * serialized with our Pack system.
 */
template<typename ReceiveType, typename TransmitType>
class Telegraph {
 private:
  ReceiveType receiveBuffer;
  Packet packetBuffer;

  Pack<ReceiveType> receiveRule;
  Pack<TransmitType> transmitRule;

 public:
  Telegraph() = delete;
  Telegraph(Pack<ReceiveType> receiveRule, Pack<TransmitType> transmitRule) :
      receiveRule(receiveRule), transmitRule(transmitRule) { }

  /**
   * Transmit a packet to one peer.
   */
  void transmit(
      Host &host, ENetPeer *const peer,
      const TransmitType &value,
      ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE) {
    packInto(transmitRule, value, packetBuffer);
    appLog(">> " + packetBuffer.dump());
    host.transmit(peer, packetBuffer.getRaw(), packetBuffer.getSize(),
                  flag);
  }

  /**
   * Transmit same packet to an iterator range of peers.
   */
  void transmitMult(
      Host &host, const boost::iterator_range<ENetPeer *> &peers,
      const TransmitType &value,
      ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE) {
    packInto(transmitRule, value, packetBuffer);
    appLog(">> " + packetBuffer.dump());
    for (ENetPeer *peer = peers.begin(); peer != peers.end(); peer++)
      host.transmit(peer, packetBuffer.getRaw(), packetBuffer.getSize(), flag);
  }

  /**
   * Transmit same packet to the peers in an iterator range of peers
   * that satisfy a predicate.
   */
  void transmitMultPred(
      Host &host, const boost::iterator_range<ENetPeer *> &peers,
      std::function<bool(ENetPeer *)> predicate,
      const TransmitType &value,
      ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE) {
    packInto(transmitRule, value, packetBuffer);
    appLog(">> " + packetBuffer.dump());
    for (ENetPeer *peer = peers.begin(); peer != peers.end(); peer++) {
      if (predicate(peer))
        host.transmit(
            peer, packetBuffer.getRaw(), packetBuffer.getSize(), flag);
    }
  }

  ReceiveType receive(const ENetPacket *packet) {
    packetBuffer.setSize(packet->dataLength);
    strcpy((char *) packetBuffer.getRaw(), (char *) packet->data);
    appLog("<< " + packetBuffer.dump());
    unpackInto(receiveRule, packetBuffer, receiveBuffer);
    return receiveBuffer;
  }
};

}

#endif //SOLEMNSKY_TELEGRAPH_H
