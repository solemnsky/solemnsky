#include "packer.h"

/****
 * Packet.
 */

Packet::Packet() { }

Packet::Packet(char data[PacketSize]) : data(data) { }

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

char Packet::readChar() {
  head++;
  return data[head - 1];
}

void Packet::writeChar(const char x) {
  data[head] = x;
  head++;
}

/****
 * Predefined rules.
 */

float PackFloat::read(Packet &packet) {
  return 0;
}

void PackFloat::write(Packet &packet, float &t) {
  ;
}

