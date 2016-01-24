/**
 * A basic system for packing and unpacking data to and from packets.
 */
#ifndef SOLEMNSKY_PACKER_H
#define SOLEMNSKY_PACKER_H

#include <string>
#include "util/types.h"

/**
 * Packet: the data, with a small interface for reading and writing primitives.
 */
class Packet {
private:
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

  char unpackChar();
  void packChar(const char x);

  void dump(); // dump to stdout
};

/**
 * PackRules: modular rules for packing and unpacking, so we
 * don't sit around writing redundant code (separate unpacking and packing
 * functions).
 */
template<typename T>
struct PackRules {
  virtual T unpack(Packet &packet) const = 0;
  virtual void pack(Packet &packet, T &t) const = 0;
};

template<typename T>
Packet pack(const PackRules<T> &rules, T x) {
  Packet packet;
  rules.pack(packet, x);
  return packet;
}

Packet pack(const PackRules<float> &rules, float x) {
  Packet packet;
  rules.pack(packet, x);
  return packet;
}

template<typename T>
T read(const PackRules<T> &rules, Packet &packet) {
  return rules.unpack(packet);
}

/**
 * Predefined PackRules.
 */
struct PackFloat : public PackRules<float> {
  float unpack(Packet &packet) const override;
  void pack(Packet &packet, float &t) const override;
};

template<typename T>
struct PackOptional : PackRules<optional<T>> {
  PackRules<T> underlyingRules;

  PackOptional<T>(PackRules<T> underlyingRules) :
      underlyingRules(underlyingRules) { }

  optional<T> read(Packet &packet) {
    char c = packet.unpackChar();
    if (c == 'y') return {underlyingRules.unpack(packet)};
    else return {};
  }

  void write(Packet &packet, optional<T> x) {
    if (x) {
      packet.packChar('y');
      underlyingRules.pack(packet, x);
    } else packet.packChar('n');
  }
};

#endif //SOLEMNSKY_PACKER_H
