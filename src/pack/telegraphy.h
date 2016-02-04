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
  enum Control {
    None, // just send it
    Arrival, // make sure they get there
    Ordering // make sure they get there in their original ordering
  } control;

  double timeout;
  // if it takes longer than this value it's no longer profitable for it to
  // arrive at all (applies to non-None controls)
};

/**
 * A thing you transmit, maps to a packet.
 */
struct Transmission {
  Transmission() {}

  Packet data;
  IpAddress destination;
};

/**
 * A thing you receive, maps to a packet.
 */
struct Reception {
  Reception(const Packet &data, const IpAddress &address);

  Packet data;
  IpAddress address;
};

namespace detail {
/**
 * Internal representation of a packet on the wire, with the data and some
 * additional information.
 */
struct WirePacket {
  WirePacket() = default;

  bool receive(sf::UdpSocket &sock,
               IpAddress &addr,
               unsigned short &port);
  void transmit(sf::UdpSocket &sock,
                const IpAddress &addr,
                const unsigned short port);

  Packet data;
};
}

class Telegraph {
private:
  sf::UdpSocket sock;
  detail::WirePacket buffer;
  std::vector<Reception> receptionCue;

public:
  Telegraph(unsigned short port);

  unsigned short port;

  /**
   * Staying alive.
   */
  void process();

  /**
   * Receptions and transmissions.
   */
  void transmit(const Transmission &transmission);
  std::vector<Reception> receive();
};

}

#endif //SOLEMNSKY_TELEGRAPHY_H
