#include "telegraphy.h"
#include "util/methods.h"

namespace pk {

namespace detail {
/**
 * WirePacket.
 */
bool WirePacket::receive(sf::UdpSocket &sock,
                         IpAddress &addr,
                         unsigned short &port) {
  static std::size_t size;
  sf::Socket::Status status =
      sock.receive(data.getRaw(), data.getSize(), size, addr, port);
  if (status == sf::Socket::Done) return true;
  if (status == sf::Socket::Error)
    appLog(LogType::Error, "packet reception error!");
  return false;
}

void WirePacket::transmit(sf::UdpSocket &sock,
                          const IpAddress &addr, const unsigned short port) {
  if (sock.send(data.getRaw(), data.getSize(), addr, port) != sf::Socket::Done)
    appLog(LogType::Error, "packet transmission error!");
}
}

/**
 * Reception.
 */
Reception::Reception(const Packet &data, const IpAddress &address) :
    data(data), address(address) { }

/**
 * Telegraph.
 */

Telegraph::Telegraph(unsigned short port) : port(port) {
  sock.bind(port);
  sock.setBlocking(false);
}

void Telegraph::process() {
  static IpAddress address;
  static unsigned short port;
  while (buffer.receive(sock, address, port)) {
    receptionCue.push_back(Reception(buffer.data, address));
  }
}

void Telegraph::transmit(const Transmission &transmission) {
}

std::vector<Reception> Telegraph::receive() {
  return receptionCue;
  receptionCue.clear();
}

}
