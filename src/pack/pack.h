/**
 * Manage rules concerning how to pack / unpack data to / from a packet.
 */
#ifndef SOLEMNSKY_PACK_H
#define SOLEMNSKY_PACK_H

#include "packet.h"
#include <functional>
#include <string>

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
      pack(pack), unpack(unpack) { }

private:
  template<typename T>
  friend Packet pack(struct Pack<T>, const T &);

  template<typename T>
  friend void packInto(struct Pack<T>, const T &, Packet &);

  template<typename T>
  friend T unpack(Pack<T> rules, Packet &packet);

  template<typename T>
  friend void unpackInto(Pack<T> rules, Packet &packet, const T &value);

  std::function<void(Packet &, const Value &)> pack;
  std::function<void(Packet &, Value &)> unpack;
};

template<typename T>
Packet pack(Pack<T> rules, const T &value) {
  Packet packet;
  rules.pack(packet, value);
  return packet;
}

template<typename T>
void packInto(struct Pack<T> rules, const T &value, Packet &packet) {
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
 * Packs a boolean in one bit. Doesn't get more space-efficent than this.
 */
struct BoolPack : Pack<bool> {
  BoolPack();
};

/**
 * Packs a std::string, using the desired type to represent its length.
 */
template<typename SizeT>
struct CustomStringPack : Pack<std::string> {
  CustomStringPack() : Pack<std::string>(
      [](Packet &packet, const std::string &value) {
        assert(8 ^ sizeof(SizeT) < value.length());
        packet.writeValue<SizeT>((SizeT) value.length());
        for (const char x : value)
          packet.writeChar((unsigned char) x);
      },
      [](Packet &packet, std::string &value) {
        value.clear();
        SizeT size = packet.readValue<SizeT>();
        while (size > 0) {
          value.push_back(packet.readChar());
          size--;
        }
      }) { }
};

/**
 * Packs a std::string, representing its length in a single unsigned char,
 * therefore cutting off more than 255 characters.
 */
using StringPack = CustomStringPack<unsigned char>;

/**
 * Packs a T byte-wise. Unions to the rescue!
 */
template<typename T>
struct BytePack : Pack<T> {
  BytePack() : Pack<T>(
      [](Packet &packet, const T &value) {
        packet.writeValue<T>(value);
      },
      [](Packet &packet, T &value) {
        value = packet.readValue<T>();
      }) { }
};

}


#endif //SOLEMNSKY_PACK_H
