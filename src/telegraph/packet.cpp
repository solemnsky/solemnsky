#include "packet.h"
#include <iostream>
#include "util/methods.h"

namespace tg {

Packet::Packet() : Packet(std::vector<unsigned char>()) { }

Packet::Packet(const std::vector<unsigned char> &data) :
    data(data) {
  this->data.resize(Packet::bufferSize);
  CTOR_LOG("Packet");
}

Packet::Packet(const Packet &packet) :
    writeHead(packet.writeHead), writeOffset(packet.writeOffset),
    data(packet.data) {
  CTOR_LOG("Packet");
}


Packet::Packet(Packet &&packet) :
    writeHead(packet.writeHead), writeOffset(packet.writeOffset),
    data(std::move(packet.data)) { }

Packet &Packet::operator=(const Packet &packet) {
  writeHead = packet.writeHead;
  writeOffset = packet.writeOffset;
  data = packet.data;
  readReset();
  return *this;
}

Packet &Packet::operator=(Packet &&packet) {
  writeHead = packet.writeHead;
  writeOffset = packet.writeOffset;
  data = std::move(packet.data); // neat eh
  readReset();
  return *this;
}

Packet::~Packet() {
  DTOR_LOG("Packet");
}

unsigned char &Packet::writeHeadData() {
  assert(writeHead < bufferSize);
  return data[writeHead];
}

/**
 * Accessing.
 */
size_t Packet::getSize() const {
  // essentially 'round up' from writeHead
  if (writeOffset == 0) return writeHead;
  else return writeHead + 1;
}

void *Packet::getRaw() {
  return data.data();
}

const void *Packet::getRaw() const {
  return data.data();
}

void Packet::setSize(size_t newSize) {
  writeHead = newSize;
}

void Packet::dumpBinary() {
  std::cout << "packet: ";
  int i;
  for (int x = 0; x < getSize(); x++) {
    for (i = 0; i < 8; i++)
      std::cout << ((data[x] >> (7 - i)) & 1);
    std::cout << ' ';
  }
  std::cout << std::endl;
}

void Packet::dump() {
  std::cout << "packet: ";
  for (int x = 0; x < getSize(); x++) std::cout << data[x];
  std::cout << std::endl;
}

/**
 * Writing.
 */

void Packet::writeReset() {
  writeHead = 0;
  writeOffset = 0;
}

void Packet::writeChar(const unsigned char x) {
  if (writeOffset == 0) {
    writeHeadData() = x;
  } else {
    writeHeadData() |= (x << writeOffset);
    writeHeadData() = x >> (8 - writeOffset);
  }
  writeHead++;
}

void Packet::writeBit(const bool x) {
  if (writeOffset == 0) {
    writeHeadData() = (unsigned char) (x ? 1 : 0);
    writeHead++;
    writeOffset++;
  } else {
    if (x) writeHeadData() |= (1 << writeOffset);
    if (writeOffset == 7) writeOffset = 0;
    else writeOffset++;
  }
}

/**
 * Reading.
 */

void Packet::readReset() {
  readOffset = 0;
  readHead = 0;
}

unsigned char Packet::readChar() {
  unsigned char result;
  if (writeOffset == 0) {
    result = data[readHead];
    readHead++;
  } else {
    result = (data[readHead] >> readOffset)
             | (data[readHead + 1] << (8 - readOffset));
    readHead++;
  }
  return result;
}

bool Packet::readBit() {
  bool result = (bool) ((data[readHead] >> readOffset) & 1);
  if (readOffset == 7) {
    readOffset = 0;
    readHead++;
  } else readOffset++;
  return result;
}

}
