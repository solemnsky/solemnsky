/**
 * A packet, 
 */
#ifndef SOLEMNSKY_PACKET_H
#define SOLEMNSKY_PACKET_H

#include "util/types.h"

namespace pk {

struct Packet {
private:
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
  Packet &operator=(const Packet&);

  /**
   * Accessing.
   */
  size_t getSize() const;
  void dumpBinary();
  void dump();

  /**
   * Writing.
   */
  void writeReset();
  void writeChar(const unsigned char x);
  void writeBit(const bool x); // actually writes a bit, yeah

  /**
   * Reading.
   */
  void readReset();
  char readChar();
  bool readBit();
};

}

#endif //SOLEMNSKY_PACKET_H
