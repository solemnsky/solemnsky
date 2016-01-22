#include "packer.h"
#include <iostream>

/****
 * Packet.
 */

Packet::Packet() { }

Packet::Packet(std::vector<char> data) : data(data) { }

int Packet::readInt() {
  union {
    char chars[sizeof(int)];
    int myInt;
  } magic;

  for (int i = 0; i < 4; i++)
    magic.chars[i] = data[head + i];

  head += 4;
  return magic.myInt;
}

void Packet::writeInt(const int x) {
  union {
    char chars[sizeof(int)];
    int myInt;
  } magic;

  magic.myInt = x;;
  for (int i = 0; i < 4; i++)
    data[head + i] = magic.chars[i];

  head += 4;
}

char Packet::unpackChar() {
  head++;
  return data[head - 1];
}

void Packet::packChar(const char x) {
  data[head] = x;
  head++;
}

void Packet::dump() {
  std::cout << "packet: ";
  for (char c : data) std::cout << c;
  std::cout << "\n";
}

/****
 * Predefined rules.
 */

float PackFloat::unpack(Packet &packet) const {
  return 0;
}

void PackFloat::pack(Packet &packet, float &t) const {
  ;
}

