/**
 * A basic system for packing and unpacking data to and from packets.
 */
#ifndef SOLEMNSKY_PACKER_H
#define SOLEMNSKY_PACKER_H

#include <string>
#include <memory>
#include "util/types.h"
#include "util/methods.h"

/****
 * Packet: the data, with a small interface for reading and writing primitives.
 */

struct Packet {
  Packet();
  Packet(std::vector<char> data);

  // reset read head on any copy or move
  Packet(const Packet &packet);
  Packet(Packet &&packet);
  Packet operator=(const Packet &packet);
  Packet operator=(Packet &&packet);

  std::vector<char> data;
  int readHead = 0;

  /**
   * Reset read head, do we ever really need to do this?
   */
  void resetHead();

  /**
   * Putting stuff in: just appends to the data.
   */
  template<typename T>
  void packValue(const T x);
  void packChar(const char x);

  /**
   * Taking stuff out: moves the read head forward.
   * (You can only read through between ctor / move / copying / resetHead()).
   */
  template<typename T>
  T unpackValue();
  char unpackChar();

  void dump(); // dump data to stdout
};

template<typename T>
void Packet::packValue(const T x) {
  union {
    char chars[sizeof(T)];
    T value;
  } magic;

  magic.value = x;

  for (size_t i = 0; i < sizeof(T); i++)
    data.push_back(magic.chars[i]);
}

template<typename T>
T Packet::unpackValue() {
  union {
    char chars[sizeof(T)];
    T value;
  } magic;

  for (size_t i = 0; i < sizeof(T); i++)
    magic.chars[i] = data.at(readHead + i);

  readHead += sizeof(T);
  return magic.value;
}

/****
 * PackRules<T>: the packing / unpacking of a T.
 */

template<typename T>
struct PackRules {
  PackRules(const std::function<void(Packet &, const T)> &pack,
            const std::function<T(Packet &)> unpack)
      : pack(pack), unpack(unpack) { }

  const std::function<void(Packet &, const T)> pack;
  const std::function<T(Packet &)> unpack;
};

template<typename T>
Packet pack(const PackRules<T> &rules, const T x) {
  Packet packet;
  rules.pack(packet, x);
  return packet;
}

template<typename T>
T unpack(const PackRules<T> &rules, Packet &packet) {
  return rules.unpack(packet);
}

/****
 * Predefined PackRules.
 */

/**
 * ValueRules: packing an intrinsic datatype.
 */
template<typename T>
class ValueRules {
  static void pack(Packet &packet, const T val) {
    packet.packValue<T>(val);
  }

  static T unpack(Packet &packet) {
    return packet.unpackValue<T>();
  }

public:
  static PackRules<T> rules() {
    return PackRules<T>(ValueRules<T>::pack, ValueRules<T>::unpack);
  }
};

template<typename T>
class OptionalRules {
  static void pack(const PackRules<T> &underlying,
                   Packet &packet, const optional<T> x) {
    if (x) {
      packet.packChar('y');
      underlying.pack(packet, *x);
    } else packet.packChar('n');
  }

  static optional<T> unpack(const PackRules<T> &underlying,
                     Packet &packet) {
    char c = packet.unpackChar();
    if (c == 'y') return {underlying.unpack(packet)};
    else return {};
  }

public:
  static PackRules<optional<T>> rules(const PackRules<T> &underlying) {
    return PackRules<optional<T>>(
        [&](Packet &packet, const optional<T> x) {
          OptionalRules<T>::pack(underlying, packet, x);
        },
        [&](Packet &packet) {
          return OptionalRules<T>::unpack(underlying, packet);
        }
    );
  }
};

#endif //SOLEMNSKY_PACKER_H
