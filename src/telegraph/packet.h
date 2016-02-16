/**
 * A packet that we can read / write bits and chars to, and presumably
 * transmit and recieve.
 */
#ifndef SOLEMNSKY_PACKET_H
#define SOLEMNSKY_PACKET_H

#include "util/types.h"

namespace tg {

/**
 * Holds datagram data. Uses only statically allocated data, provides easy
 * handles to recv()/send() style APIs. Hold Packets as consistently
 * allocated buffers when possible.
 */
struct Packet {
public:
  static constexpr size_t bufferSize = 1024 * 2;

  Packet();

  Packet(const Packet &packet);
  Packet &operator=(const Packet &);

  ~Packet();

  /**
   * Data.
   */
  std::array<unsigned char, bufferSize> data;
  // statically allocated data
  size_t size; // data that is actually used
  void reset();

  /**
   * Debug dumping.
   */
  std::string dumpBinary() const;
  std::string dump() const;

  /**
   * Using recv() / send() API.
   */
  unsigned char *getRaw();
  size_t getSize();
  void setSize(size_t size);
};

/**
 * 'offset' and 'head' variables in the following code represent this:
 *
 * 1: written / read, 0: unwritten / unread
 * [...] 11111111 00001111 00000000 [...]
 *                   ^ offset = 4
 *                ^ head
 */

/**
 * Write stuff to a packet, bit-vector style.
 */
struct PacketWriter {
private:
  Packet *packet;
  size_t head;
  unsigned char offset;

  unsigned char &accessHead();

public:
  PacketWriter(Packet *packet);

  void writeChar(const unsigned char x);
  void writeBit(const bool x); // by the power of bitwise operation
  template<typename T>
  void writeValue(const T x);
};

/**
 * Read stuff from a packet, bit-vector style.
 */
struct PacketReader {
private:
  const Packet *packet;
  size_t head;
  unsigned char offset;

  optional<unsigned char> accessHead() const;

public:
  PacketReader(const Packet *packet);

  optional<unsigned char> readChar();
  optional<bool> readBit();
  template<typename T>
  optional<T> readValue();
};

/**
 * Templated definitions for writeValue and readValue.
 */

template<typename T>
void PacketWriter::writeValue(const T x) {
  union {
    unsigned char chars[sizeof(T)];
    T value;
  } magic;

  magic.value = x;
  for (size_t i = 0; i < sizeof(T); i++) writeChar(magic.chars[i]);
}

template<typename T>
optional<T> PacketReader::readValue() {
  union {
    unsigned char chars[sizeof(T)];
    T value;
  } magic;

  optional<unsigned char> currChar;
  for (size_t i = 0; i < sizeof(T); i++) {
    currChar = readChar();
    if (!currChar) return {};
    magic.chars[i] = *currChar;
  }
  return magic.value;
}

}

#endif //SOLEMNSKY_PACKET_H
