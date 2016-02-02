#include "server.h"
#include "pack/pack.h"
#include <iostream>

int main() {
  pk::Pack<float> floatPack = pk::BytePack<float>();

  pk::Packet packet;

  packet = pk::pack(floatPack, 50.5f);
  packet.dumpBinary();
  std::cout << pk::unpack(floatPack, packet);
}
