#include "packet.h"
#include <iostream>

namespace pk {

Packet::Packet() : Packet(std::vector<unsigned char>()) {
  data.resize(Packet::bufferSize);
}

Packet::Packet(const std::vector<unsigned char> &data) :
    data(data),
    writeOffset(0),
    readOffset(0),
    readHead(0) {
  this->data.resize(Packet::bufferSize);
}

Packet::Packet(const Packet &packet) :
    Packet(packet.data) { }

Packet &Packet::operator=(const Packet &packet) {
  writeOffset = packet.writeOffset;
  data = packet.data;
  readReset();
  return *this;
}

/**
 * Accessing.
 */
size_t Packet::getSize() const {
  return data.size();
}

void *Packet::getRaw() {
  return data.data();
}

void Packet::dumpBinary() {
  std::cout << "packet: ";
  int i;
  for (char x : data) {
    for (i = 0; i < 8; i++)
      std::cout << ((x >> (7 - i)) & 1);
    std::cout << ' ';
  }
  std::cout << '\n';
}

void Packet::dump() {
  std::cout << "packet: ";
  for (char x : data) std::cout << x;
  std::cout << '\n';
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
    data[writeHead] = x;
  } else {
    writeHeadData() |= (x << writeOffset);
    data[writeHead] = x >> (8 - writeOffset);
  }
  writeHead++;
}

void Packet::writeBit(const bool x) {
  if (writeOffset == 0) {
    data[writeHead] = (unsigned char) (x ? 1 : 0);
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
