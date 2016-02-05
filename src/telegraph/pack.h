/**
 * Manage rules concerning how to pack / unpack data to / from a packet.
 */
#ifndef SOLEMNSKY_PACK_H
#define SOLEMNSKY_PACK_H

#include "packet.h"
#include <functional>
#include <string>

namespace tg { // TeleGraph

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
  friend Packet pack(const Pack<T> &, const T &);

  template<typename T>
  friend void packInto(const Pack<T> &, const T &, Packet &);

  template<typename T>
  friend T unpack(const Pack<T> &, Packet &);

  template<typename T>
  friend void unpackInto(const Pack<T> &, Packet &, T &);

  const std::function<void(Packet &, const Value &)> pack;
  const std::function<void(Packet &, Value &)> unpack;
};

template<typename T>
Packet pack(const Pack<T> &rules, const T &value) {
  Packet packet;
  rules.pack(packet, value);
  return packet;
}

template<typename T>
void packInto(const Pack<T> &rules, const T &value, Packet &packet) {
  rules.pack(packet, value);
}

template<typename T>
T unpack(const Pack<T> &rules, Packet &packet) {
  T value;
  rules.unpack(packet, value);
  return value;
}

template<typename T>
void unpackInto(const Pack<T> &rules, Packet &packet, T &value) {
  rules.unpack(packet, value);
}

/****
 * Pack<> implementations.
 */

/**
 * Pack a boolean in one bit. Doesn't get more space-efficent than this.
 */
struct BoolPack : Pack<bool> {
  BoolPack();
};

/**
 * Pack an enum using a variable number of bits. Don't take more than you use.
 */
template<typename Enum>
struct EnumPack : Pack<Enum> {
  EnumPack<Enum>(int bits) : Pack<Enum>(
      [&bits](Packet &packet, const Enum &value) {
        for (unsigned char i = 0; i < bits; i++)
          packet.writeBit((const bool) ((((unsigned char) value) >> i) & 1));
      },
      [&bits](Packet &packet, Enum &value) {
        for (unsigned char i = 0; i < bits; i++)
          if (packet.readBit())
            value = static_cast<Enum>(
                        (((unsigned char) value) | (1 << i)));
      }) { }
};

/**
 * Pack a std::string, using the desired type to represent its length.
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
 * Pack a std::string, representing its length in a single unsigned char,
 * therefore cutting off more than 255 characters.
 */
using StringPack = CustomStringPack<unsigned char>;

/**
 * Pack a T byte-wise. Unions to the rescue!
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

/**
 * Packs an optional<>.
 */
template<typename T>
struct OptionalPack : Pack<optional<T>> {
  OptionalPack(const Pack<T> &rule) : Pack<optional<T>>(
      [rule](Packet &packet, const optional<T> &value) {
        packet.writeBit((bool) value);
        if (value) packInto(rule, *value, packet);
      },
      [rule](Packet &packet, optional<T> &value) {
        if (packet.readBit())
          value.emplace(unpack(rule, packet));
        else value.reset();
      }) { }
};

/**
 * Pack a class, syncing an variadic argument list of
 */
template<typename Class, typename Member>
struct MemberRule {
  MemberRule(const Pack<Member> &rule, Member Class::* const ptr) :
      rule(rule), ptr(ptr) { }

  const Pack<Member> rule;
  Member Class::* const ptr;
};

template<typename Class>
struct ClassPack : Pack<Class> {
private:
  static void classPack(Packet &packet, const Class &value) { }

  template<typename Member, typename... Rest>
  static void classPack(Packet &packet, const Class &value,
                        const MemberRule<Class, Member> &rule, Rest... rest) {
    packInto(rule.rule, value.*rule.ptr, packet);
    classPack(packet, value, rest...);
  };

  static void classUnpack(Packet &packet, Class &value) { }

  template<typename Member, typename... Rest>
  static void classUnpack(Packet &packet, Class &value,
                          const MemberRule<Class, Member> &rule, Rest... rest) {
    unpackInto(rule.rule, packet, value.*rule.ptr);
    classUnpack(packet, value, rest...);
  };

public:
  template<typename... Members>
  ClassPack(Members... members) : Pack<Class>(
      [members...](Packet &packet, const Class &value) {
        ClassPack::classPack(packet, value, members...);
      },
      [members...](Packet &packet, Class &value) {
        ClassPack::classUnpack(packet, value, members...);
      }) { }
};

}


#endif //SOLEMNSKY_PACK_H
