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

  inline unsigned char &lastData() { return data[data.size() - 1]; }

  int writeOffset; // offset when writing (appending) to
  int readHead, readOffset; // head and offset when reading through

  /**
   * During writing:
   *
   * 1: written, 0: unwritten
   * [...] 11111111 00001111
   *                   ^ writeOffset = 4
   */

  /**
   * During reading:
   *
   * 1: read, 0: unread
   * 11111111 00001111 [...]
   *             ^ read head = 4
   *          ^ readOffset = 1
   */

public:
  Packet();
  Packet(const std::vector<unsigned char> &data);

  Packet(const Packet &);
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

  /**
   * Writing other byte-construtable values like floats and ints.
   */
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
