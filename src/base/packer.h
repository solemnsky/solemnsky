/**
 * A basic system for packing and unpacking data to and from packets.
 */
#ifndef SOLEMNSKY_PACKER_H
#define SOLEMNSKY_PACKER_H

#include <string>
#include "sysutil.h"

/**
 * Packet: the data, with a small interface for reading and writing primitives.
 */
class Packet {
private:
  enum {
    PacketSize = 1024
  };  // TODO: should I be doing this?

  int head = 0;
public:
  Packet();
  Packet(std::vector<char> data);

  std::vector<char> data;

  /**
   * Helper readers / writers.
   */
  int readInt();
  void writeInt(const int x);

  char readChar();
  void writeChar(const char x);
};

/**
 * PackRules: modular rules for packing and unpacking, so we
 * don't sit around writing redundant code (separate unpacking and packing
 * functions).
 */
template<typename T>
struct PackRules {
  virtual T read(Packet &packet) = 0;
  virtual void write(Packet &packet, T &t) = 0;
};

template<typename T>
Packet pack(PackRules<T> rules, T x) {
  Packet packet;
  rules.write(packet, x);
  return packet;
}

template<typename T>
T read(PackRules<T> rules, Packet &packet) {
  return rules.read(packet);
}

/**
 * Predefined PackRules.
 */
struct PackFloat : PackRules<float> {
  float read(Packet &packet) override;
  void write(Packet &packet, float &t) override;
};

template<typename T>
struct PackOptional : PackRules<optional<T>> {
  PackRules<T> underlyingRules;

  PackOptional<T>(PackRules<T> underlyingRules) :
      underlyingRules(underlyingRules) { }

  optional<T> read(Packet &packet) {
    char c = packet.readChar();
    if (c == 'y') return {underlyingRules.read(packet)};
    else return {};
  }

  void write(Packet &packet, optional<T> x) {
    if (x) {
      packet.writeChar('y');
      underlyingRules.write(packet, x);
    } else packet.writeChar('n');
  }
};

#endif //SOLEMNSKY_PACKER_H
