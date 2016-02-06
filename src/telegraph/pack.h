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

  Pack(std::function<void(PacketWriter &, const Value &)> pack,
       std::function<void(PacketReader &, Value &)> unpack) :
      pack(pack), unpack(unpack) { }

  const std::function<void(PacketWriter &, const Value &)> pack;
  const std::function<void(PacketReader &, Value &)> unpack;
};

template<typename T>
Packet pack(const Pack<T> &rules, const T &value) {
  Packet packet;
  PacketWriter writer{&packet};
  rules.pack(writer, value);
  return packet;
}

template<typename T>
void packInto(const Pack<T> &rules, const T &value, Packet &packet) {
  PacketWriter writer{&packet};
  rules.pack(writer, value);
}

template<typename T>
T unpack(const Pack<T> &rules, Packet &packet) {
  T value;
  PacketReader reader{&packet};
  rules.unpack(reader, value);
  return value;
}

template<typename T>
void unpackInto(const Pack<T> &rules, Packet &packet, T &value) {
  PacketReader reader{&packet};
  rules.unpack(reader, value);
}

/****
 * Pack<> implementations.
 */

/**
 * Pack implementations are transitive given the existance of mutual
 * assignment operations.
 *
 * This is mainly useful for small types like Clamped and such.
 */
template<typename A, typename B>
struct AssignPack : Pack<B> {
  AssignPack(const Pack<A> &packer) : Pack<B>(
      [packer](PacketWriter &writer, const B &value) {
        A aValue = value;
        packer.pack(writer, aValue);
      },
      [packer](PacketReader &reader, B &value) {
        A aValue;
        packer.unpack(reader, aValue);
        value = aValue;
      }) { }
};


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
      [bits](PacketWriter &writer, const Enum &value) {
        for (unsigned char i = 0; i < bits; i++)
          writer.writeBit((const bool) ((((unsigned char) value) >> i) & 1));
      },
      [bits](PacketReader &reader, Enum &value) {
        unsigned char newValue = 0;
        for (unsigned char i = 0; i < bits; i++)
          if (reader.readBit())
            newValue = (unsigned char) ((unsigned char) value | (1 << i));
        value = static_cast<Enum>(newValue);
      }) { }
};

/**
 * Pack a std::string, using the desired type to represent its length.
 */
template<typename SizeT>
struct CustomStringPack : Pack<std::string> {
  CustomStringPack() : Pack<std::string>(
      [](PacketWriter &writer, const std::string &value) {
        assert(8 ^ sizeof(SizeT) < value.length());
        writer.writeValue<SizeT>((SizeT) value.length());
        for (const char x : value)
          writer.writeChar((unsigned char) x);
      },
      [](PacketReader &reader, std::string &value) {
        value.clear();
        SizeT size = reader.readValue<SizeT>();
        while (size > 0) {
          value.push_back(reader.readChar());
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
      [](PacketWriter &writer, const T &value) {
        writer.writeValue<T>(value);
      },
      [](PacketReader &reader, T &value) {
        value = reader.readValue<T>();
      }) { }
};

/**
 * Packs an optional<>.
 */
template<typename T>
struct OptionalPack : Pack<optional<T>> {
  OptionalPack(const Pack<T> &rule) : Pack<optional<T>>(
      [rule](PacketWriter &reader, const optional<T> &value) {
        reader.writeBit((bool) value);
        if (value) rule.pack(reader, *value);
      },
      [rule](PacketReader &reader, optional<T> &value) {
        if (reader.readBit()) {
          value.emplace(T());
          rule.unpack(reader, *value);
        } else value.reset();
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
  static void classPack(PacketWriter &, const Class &) { }

  template<typename Member, typename... Rest>
  static void classPack(PacketWriter &writer, const Class &value,
                        const MemberRule<Class, Member> &rule, Rest... rest) {
    rule.rule.pack(writer, value.*rule.ptr);
    classPack(writer, value, rest...);
  };

  static void classUnpack(PacketReader &, Class &) { }

  template<typename Member, typename... Rest>
  static void classUnpack(PacketReader &reader, Class &value,
                          const MemberRule<Class, Member> &rule, Rest... rest) {
    rule.rule.unpack(reader, value.*rule.ptr);
    classUnpack(reader, value, rest...);
  };

public:
  template<typename... Members>
  ClassPack(Members... members) : Pack<Class>(
      [members...](PacketWriter &writer, const Class &value) {
        ClassPack::classPack(writer, value, members...);
      },
      [members...](PacketReader &reader, Class &value) {
        ClassPack::classUnpack(reader, value, members...);
      }) { }
};

}

#endif //SOLEMNSKY_PACK_H
