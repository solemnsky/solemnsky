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
 * Wire.
 */
Wire::Wire(Packet *buffer) : buffer(buffer) { }

bool Wire::receive(sf::UdpSocket &sock,
                   IpAddress &addr,
                   unsigned short &port) {
  static std::size_t size;
  sf::Socket::Status status =
      sock.receive(buffer->getRaw(), Packet::bufferSize, size, addr, port);
  if (size == 0) return false;

  buffer->setSize(size);
  if (status == sf::Socket::Error)
    appLog(LogType::Error, "packet reception error!");
  return true;
}

void Wire::transmit(sf::UdpSocket &sock,
                    const IpAddress &addr,
                    const unsigned short port) {
  if (sock.send(buffer->getRaw(), buffer->getSize(), addr, port) !=
      sf::Socket::Done)
    appLog(LogType::Error, "packet transmission error!");
}
}

}
