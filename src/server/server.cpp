#include "server.h"
#include "pack/packet.h"
#include <iostream>

int main() {
  pk::Packet packet;
  packet.writeBit(1);
  packet.writeBit(1);
  packet.writeChar('a');
  packet.writeBit(0);
  packet.writeChar('z');
  packet.writeBit(1);
  packet.writeChar('f');
  packet.dump();
  std::cout << packet.readBit();
  std::cout << packet.readBit();
  std::cout << packet.readChar();
  std::cout << packet.readBit();
  std::cout << packet.readChar();
  std::cout << packet.readBit();
  std::cout << packet.readChar();
}
