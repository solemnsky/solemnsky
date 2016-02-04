/**
 * Manager sending / receiving packets with various transmission control
 * strategies. Used both as a listener and as a transmitter. Every UDP
 * connection in our network design has one of these at both ends.
 *
 * Yeah, Telegraph. That's what I'm calling it. You can't argue with the Latin.
 * At least I didn't call it SemaphoreLine or something.
 */

#ifndef SOLEMNSKY_TELEGRAPHY_H
#define SOLEMNSKY_TELEGRAPHY_H

#include "pack.h"
#include <SFML/Network.hpp>

namespace pk {

using IpAddress = sf::IpAddress;

/**
 * Transmission strategy: describes the system controls a
 * particular transmission. For instance, a chat packet needs ordering and
 * arrival verification, but a HUD update packet shouldn't take up extra
 * bandwidth to verify arrival or ordering because after 100 ms it's worthless.
 */
struct Strategy {
  enum class Control {
    None, // just send it
    Arrival, // make sure they get there
    Ordering // make sure they get there in their original ordering
  };

  Strategy(const Control = Control::None,
           const optional<double> timeout = {});

  Control control;
  optional<double> timeout;
  // if it takes longer than this value it's no longer profitable for it to
  // arrive at all (applies to non-None controls)
};

/**
 * Transmission. Parameter holder for Telegraph::transmit().
 */
struct Transmission {
  Transmission(Packet &&packet, // *move, not copy*!
               const IpAddress &destination,
               const Strategy &strategy);

  Packet packet;
  IpAddress destination;
  Strategy strategy;
};

/**
 * Reception. Parameter holder for Telegraph::receive() callback.
 */
struct Reception {
  Reception(Packet *packet,
            const IpAddress &address);

  Packet *packet; // only access this in the callback
  IpAddress address;
};

namespace detail {
/**
 * Internal representation of a packet on the wire, with packet data and
 * meta-information.
 */
struct WirePacket {
  WirePacket() = default;
  WirePacket(Packet &&packet); // *move, not copy*!

  bool receive(sf::UdpSocket &sock,
               IpAddress &addr,
               unsigned short &port);
  void transmit(sf::UdpSocket &sock,
                const IpAddress &addr,
                const unsigned short port);

  Packet packet;
  // TODO: strategy-related meta-information
};
}

class Telegraph {
private:
  sf::UdpSocket sock;

public:
  Telegraph(unsigned short port);
  const unsigned short port;

  /**
   * Receptions and transmissions.
   */
  void transmit(Transmission &&transmission);
  void receive(std::function<void(Reception &)> onReceive);
  // continually call this
};
}

#endif //SOLEMNSKY_TELEGRAPHY_H
