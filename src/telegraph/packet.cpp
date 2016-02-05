#include "packet.h"
#include <iostream>
#include "util/methods.h"

namespace tg {

/**
 * Packet.
 */
Packet::Packet() : size(0) { }

Packet::Packet(const Packet &packet) :
    data(packet.data), size(packet.size) { CTOR_LOG("Packet"); }

Packet::Packet(Packet &&packet) :
    data(std::move(packet.data)), size(packet.size) { CTOR_LOG("Packet"); }

Packet &Packet::operator=(const Packet &packet) {
  data = packet.data;
  size = packet.size;
  return *this;
}

Packet &Packet::operator=(Packet &&packet) {
  data = std::move(packet.data); // neat eh
  size = packet.size;
  return *this;
}

Packet::~Packet() {
  DTOR_LOG("Packet");
}

void Packet::dumpBinary() {
  std::cout << "packet: ";
  int i;
  for (int x = 0; x < size; x++) {
    for (i = 0; i < 8; i++)
      std::cout << ((data[x] >> (7 - i)) & 1);
    std::cout << ' ';
  }
  std::cout << std::endl;
}

void Packet::dump() {
  std::cout << "packet: ";
  for (int x = 0; x < size; x++) std::cout << data[x];
  std::cout << std::endl;
}

/**
 * PacketWriter.
 */
PacketWriter::PacketWriter(Packet *packet) :
    packet(packet), head(0), offset(0) { }

unsigned char &PacketWriter::accessHead() {
  if (head >= Packet::bufferSize) {
    appErrorLogic(
        "We can't write that much to a packet! "
            "It's time to implement packet fragmentation, you lazy fool.");
  }
  return packet->data[head];
}

void PacketWriter::writeChar(const unsigned char x) {
  unsigned char &headRef = accessHead();
  head++;
  if (offset == 0) {
    headRef = x;
  } else {
    headRef |= (x << offset);
    head++;
    accessHead() = x >> (8 - offset);
  }
}

void PacketWriter::writeBit(const bool x) {
  unsigned char &head = accessHead();
  if (offset == 0) {
    accessHead() = (unsigned char) (x ? 1 : 0);
    head++;
    offset++;
  } else {
    if (x) accessHead() |= (1 << offset);
    if (offset == 7) offset = 0;
    else offset++;
  }
}

/**
 * PacketReader.
 */

PacketReader::PacketReader(Packet const *packet) :
    packet(packet), head(0), offset(0) { }

unsigned char PacketReader::accessHead() const {
  if (head >= Packet::bufferSize) {
    appErrorLogic(
        "We can't write that much to a packet! "
            "It's time to implement packet fragmentation, you lazy fool.");
  }
  return packet->data[head];
}

unsigned char PacketReader::readChar() {
  const unsigned char headVal = accessHead();
  head++;
  if (offset == 0) {
    return headVal;
  } else {
    return (headVal >> offset)
           | (accessHead() << (8 - offset));
  }
}

bool PacketReader::readBit() {
  bool result = (bool) ((accessHead() >> offset) & 1);
  if (offset == 7) {
    offset = 0;
    head++;
  } else offset++;
  return result;
}

}
