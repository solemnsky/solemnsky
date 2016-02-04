/**
 * A packet that we can read / write bits and chars to, and presumably
 * transmit and recieve.
 */
#ifndef SOLEMNSKY_PACKET_H
#define SOLEMNSKY_PACKET_H

#include "util/types.h"

namespace pk {

struct Packet {
private:
  static constexpr int bufferSize = 1024*2; // magic number

  std::vector<unsigned char> data;

  inline unsigned char &writeHeadData() { return data[writeHead]; }

  int writeHead, writeOffset; // offset when writing through
  int readHead, readOffset; // head and offset when reading through

  /**
   * 1: written / read, 0: unwritten / unread
   * [...] 11111111 00001111 00000000 [...]
   *                   ^ offset = 4
   *                ^ writeHead
   */

public:
  Packet();
  Packet(const std::vector<unsigned char> &data);

  Packet(const Packet &packet);
  Packet &operator=(const Packet &);

  /**
   * Accessing.
   */
  size_t getSize() const;
  void *getRaw();

  void dumpBinary();
  void dump();

  /**
   * Writing.
   */
  void writeReset();
  void writeChar(const unsigned char x);
  void writeBit(const bool x); // by the power of bitwise operation

  template<typename T>
  void writeValue(const T x);

  /**
   * Reading.
   */
  void readReset();
  unsigned char readChar();
  bool readBit();

  template<typename T>
  T readValue();
};

template<typename T>
void Packet::writeValue(const T x) {
  union {
    unsigned char chars[sizeof(T)];
    T value;
  } magic;

  magic.value = x;

  for (size_t i = 0; i < sizeof(T); i++)
    writeChar(magic.chars[i]);
}

template<typename T>
T Packet::readValue() {
  union {
    unsigned char chars[sizeof(T)];
    T value;
  } magic;

  for (size_t i = 0; i < sizeof(T); i++)
    magic.chars[i] = readChar();

  return magic.value;
}

}

#endif //SOLEMNSKY_PACKET_H
