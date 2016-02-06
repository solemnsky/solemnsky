#include "telegraph.h"
#include "util/methods.h"

namespace tg {

/**
 * Strategy.
 */
Strategy::Strategy(const Strategy::Control control,
                   const optional<double> timeout) :
    control(control), timeout(timeout) { }

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

}
