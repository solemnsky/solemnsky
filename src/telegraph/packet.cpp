#include "packet.h"
#include <iostream>
#include "util/methods.h"

namespace tg {

/**
 * Packet.
 */
Packet::Packet() : size(0) { }

Packet::Packet(const Packet &packet) :
    size(packet.size) { data = packet.data; }

Packet &Packet::operator=(const Packet &packet) {
  data = packet.data;
  size = packet.size;
  return *this;
}

Packet::~Packet() { }

void Packet::reset() {
  size = 0;
}

std::string Packet::dumpBinary() const {
  std::string str;
  int i;
  for (int x = 0; x < size; x++) {
    for (i = 0; i < 8; i++)
      str += ((bool) ((data[x] >> (7 - i)) & 1)) ? '1' : '0';
    str += " ";
  }
  return str;
}

std::string Packet::dump() const {
  std::string str;
  for (int x = 0; x < size; x++) str += (char) data[x];
  return str;
}

unsigned char *Packet::getRaw() {
  return data.data();
}

size_t Packet::getSize() {
  return size;
}

void Packet::setSize(size_t size) {
  this->size = size;
}

/**
 * PacketWriter.
 */
PacketWriter::PacketWriter(Packet *packet) :
    packet(packet), head(0), offset(0) {
  packet->size = 0;
}

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
  packet->size++;
  if (offset == 0) {
    headRef = x;
  } else {
    headRef |= (x << offset);
    accessHead() = x >> (8 - offset);
  }
}

void PacketWriter::writeBit(const bool x) {
  unsigned char &headRef = accessHead();
  if (offset == 0) {
    headRef = (unsigned char) (x ? 1 : 0);
    offset++;
    packet->size++;
  } else {
    if (x) headRef |= (1 << offset);
    if (offset == 7) {
      offset = 0;
      head++;
    }
    else offset++;
  }
}

/**
 * PacketReader.
 */

PacketReader::PacketReader(const Packet *packet) :
    packet(packet), head(0), offset(0) { }

unsigned char PacketReader::accessHead() const {
  if (head >= packet->size) {
    appErrorLogic(
        "Trying to read too much from a packet!");
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
