/**
 * Manage rules concerning how to pack / unpack data to / from a packet.
 */
#ifndef SOLEMNSKY_PACK_H
#define SOLEMNSKY_PACK_H

#include "packet.h"
#include <functional>

namespace pk {

/**
 * Non-abstract struct holding rules on how to pack / unpack a Value.
 * For API consistency, functionality is hidden behind non-member methods pack,
 * packInto, unpack, and unpackInfo.
 */
template<typename Value>
struct Pack {
  Pack() = delete;
  Pack(std::function<void(Packet &, const Value &)> pack,
       std::function<void(Packet &, Value &)> unpack) :
      pack(pack), unpack(unpack) {}

private:
  friend template<typename T>
  Packet pack(struct Pack<T>, const T &);

  friend template<typename T>
  void packInto(struct Pack<T>, const T &, Packet &);

  friend template<typename T>
  T unpack(Pack<T> rules, Packet &packet);

  friend template<typename T>
  void unpackInto(Pack<T> rules, Packet &packet, const T &value);

  std::function<void(Packet &, const Value &)> pack;
  std::function<void(Packet &, Value &)> unpack;
};

template<typename T>
Packet pack(Pack<T> rules, const T & value) {
  Packet packet;
  rules.pack(packet, value);
  return packet;
}

template<typename T>
void packInto(struct Pack<T> rules, const T & value, Packet & packet) {
  rules.pack(packet, value);
}

template<typename T>
T unpack(Pack<T> rules, Packet &packet) {
  T value;
  rules.unpack(packet, value);
  return value;
}

template<typename T>
void unpackInto(Pack<T> rules, Packet &packet, const T &value) {
  rules.unpack(packet, value);
}

/****
 * Pack<> implementations.
 */

/**
 * Packs a T by reading and writings its bytes, in a union.
 */
template<typename T>
struct BytePack : Pack<T> {

};

}


#endif //SOLEMNSKY_PACK_H
