/**
 * Here we define an infrastructure for sending / receiving packets with
 * various transmission control strategies.
 *
 * Yeah, Telegraph. That's what I'm calling it. You can't argue with the Latin.
 * At least I didn't call it SemaphoreLine or something.
 */

#ifndef SOLEMNSKY_TELEGRAPHY_H
#define SOLEMNSKY_TELEGRAPHY_H

#include "pack.h"
#include <SFML/Network.hpp>

namespace tg {

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
 * Transmission. Parameter / reference holder for Telegraph::transmit().
 */
struct Transmission {
  Transmission(Packet &packet,
               const IpAddress &destination,
               const unsigned short port,
               const Strategy &strategy = Strategy());

  Packet &packet;
  const IpAddress destination;
  const Strategy strategy;
  const unsigned short port;
};

/**
 * Reception. Parameter / reference holder for Telegraph::receive() callback.
 */
struct Reception {
  Reception(Packet &packet,
            const IpAddress &address);

  Packet &packet;
  const IpAddress address;
};

namespace detail {
/**
 * Helper construct to manage receiving and transmitting packets, along with
 * metadata and stuff.
 */
struct WirePacket {
  WirePacket() = default;
  WirePacket(Packet *packet);

  bool receive(sf::UdpSocket &sock,
               IpAddress &addr,
               unsigned short &port);
  void transmit(sf::UdpSocket &sock,
                const IpAddress &addr,
                const unsigned short port);

  Packet *packet;
  // TODO: strategy-related meta-information
};
}

/**
 * Transmission manager of sorts, used both as a listener and as a transmitter.
 * Every UDP connection in our design has one of these at both ends.
 */
class Telegraph {
private:
  sf::UdpSocket sock;
  Packet incomingBuffer;

public:
  Telegraph(unsigned short port);
  const unsigned short port;

  /**
   * Receptions and transmissions.
   */
  void transmit(Transmission &&transmission);
  void receive(std::function<void(Reception &&)> onReceive);
};
}

#endif //SOLEMNSKY_TELEGRAPHY_H
