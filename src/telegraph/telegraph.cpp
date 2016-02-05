#include "telegraph.h"
#include "util/methods.h"

namespace tg {

/**
 * Strategy.
 */
Strategy::Strategy(const Strategy::Control control,
                   const optional<double> timeout) :
    control(control), timeout(timeout) { }

/**
 * Transmission.
 */
Transmission::Transmission(Packet &packet, const IpAddress &destination,
                           const unsigned short port, const Strategy &strategy)
    :
    packet(packet), destination(destination),
    strategy(strategy), port(port) { }

/**
 * Reception.
 */
Reception::Reception(Packet &packet,
                     const IpAddress &address) :
    packet(packet), address(address) { }

namespace detail {
/**
 * WirePacket.
 */
WirePacket::WirePacket(Packet *packet) :
    packet(packet) { }

bool WirePacket::receive(sf::UdpSocket &sock,
                         IpAddress &addr,
                         unsigned short &port) {
  static std::size_t size;
  sf::Socket::Status status =
      sock.receive(packet->getRaw(), Packet::bufferSize, size, addr, port);
  if (size == 0) return false;

  packet->setSize(size);
  if (status == sf::Socket::Error)
    appLog(LogType::Error, "packet reception error!");
  return true;
}

void WirePacket::transmit(sf::UdpSocket &sock,
                          const IpAddress &addr,
                          const unsigned short port) {
  if (sock.send(packet->getRaw(), packet->getSize(), addr, port) !=
      sf::Socket::Done)
    appLog(LogType::Error, "packet transmission error!");
}

}

/**
 * Telegraph.
 */
Telegraph::Telegraph(unsigned short port) : port(port) {
  sock.bind(port);
  sock.setBlocking(false);
}

void Telegraph::transmit(Transmission &&transmission) {
  static detail::WirePacket wire(&transmission.packet);
  // wire.header = blah blah
  wire.transmit(sock, transmission.destination, transmission.port);
}

void Telegraph::receive(std::function<void(Reception &&)> onReceive) {
  detail::WirePacket wire(&incomingBuffer);
  static IpAddress address;
  static unsigned short port;
  while (wire.receive(sock, address, port)) {
    onReceive(Reception(incomingBuffer, address));
    // potentially respond to server or don't immediately add to cue
    // according to strategy in use
  }
}
}
