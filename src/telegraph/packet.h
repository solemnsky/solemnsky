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
  std::array<unsigned char, bufferSize> data; // statically allocated data
  size_t size; // data that is actually used

  /**
   * Dumping to stdout.
   */
  void dumpBinary();
  void dump();

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
  Packet const *packet;
  size_t head;
  unsigned char offset;

  unsigned char accessHead() const;

public:

  PacketReader(Packet const *packet);

  unsigned char readChar();
  bool readBit();
  template<typename T>
  T readValue();
};

/**
 * Template definition for writeValue and readValue.
 */

template<typename T>
void PacketWriter::writeValue(const T x) {
  union {
    unsigned char chars[sizeof(T)];
    T value;
  } magic;

  magic.value = x;

  for (size_t i = 0; i < sizeof(T); i++)
    writeChar(magic.chars[i]);
}

template<typename T>
T PacketReader::readValue() {
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
