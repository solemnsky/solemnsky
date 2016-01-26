#include "packer.h"
#include <iostream>

/****
 * Packet.
 */

Packet::Packet() { }

Packet::Packet(std::vector<char> data) : data(data) { }

Packet::Packet(const Packet &packet) : data(packet.data) { }

Packet::Packet(Packet &&packet) : data(packet.data) { }

Packet Packet::operator=(const Packet &packet) {
  readHead = 0;
  data = packet.data;
  return *this;
}

Packet Packet::operator=(Packet &&packet) {
  readHead = 0;
  data = packet.data;
  return *this;
}

void Packet::resetHead() {
  readHead = 0;
}

char Packet::unpackChar() {
  readHead++;
  return data[readHead - 1];
}

void Packet::packChar(const char x) {
  data.push_back(x);
}

void Packet::dump() {
  std::cout << "packet: ";
  for (char c : data) std::cout << c;
  std::cout << "\n";
}
