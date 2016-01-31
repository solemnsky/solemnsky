#include "packer.h"
#include <iostream>

/****
 * Packet.
 */

Packet::Packet() { CTOR_LOG("packet"); }

Packet::Packet(const std::vector<char> &data) :
    data(data) { CTOR_LOG("packet"); }

Packet::Packet(const Packet &packet) : Packet(packet.data) { }

Packet::Packet(Packet &&packet) : Packet(packet.data) { }

Packet::~Packet() { DTOR_LOG("packet"); }

Packet & Packet::operator=(const Packet &packet) {
  readHead = 0;
  data = packet.data;
  return *this;
}

Packet & Packet::operator=(Packet &&packet) {
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

void Packet::reset() {
  data.clear();
  resetHead();
}
