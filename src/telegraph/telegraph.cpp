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
  appLog("incoming packet: " + buffer->dump());

  buffer->setSize(size);
  if (status == sf::Socket::Error)
    appErrorRuntime("packet reception error!");
  return true;
}

void Wire::transmit(sf::UdpSocket &sock,
                    const IpAddress &addr,
                    const unsigned short port) {
  appLog("outgoing packet: " + buffer->dump());
  if (sock.send(buffer->getRaw(), buffer->getSize(), addr, port) !=
      sf::Socket::Done)
    appErrorRuntime("packet transmission error!");
}
}

}
