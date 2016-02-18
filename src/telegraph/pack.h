/**
 * Manage rules concerning how to pack / unpack data to / from a packet.
 */
#ifndef SOLEMNSKY_PACK_H
#define SOLEMNSKY_PACK_H

#include "packet.h"
#include <list>
#include <functional>
#include "util/methods.h"
#include <map>
#include <string>

namespace tg { // TeleGraph

/**
 * Non-abstract struct holding rules on how to pack / unpack a Value.
 */
template<typename Value>
struct Pack {
  Pack() = delete;

  Pack(std::function<void(PacketWriter &, const Value &)> pack,
       std::function<bool(PacketReader &, Value &)> unpack) :
      pack(pack), unpack(unpack) { }

  const std::function<void(PacketWriter &, const Value &)> pack;
  // returns true <=> unpacking succeeded <= packet is not corrupted
  const std::function<bool(PacketReader &, Value &)> unpack;
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
optional<T> unpack(const Pack<T> &rules, const Packet &packet) {
  T value;
  PacketReader reader{&packet};
  if (rules.unpack(reader, value)) return value;
  return {};
}

template<typename T>
bool unpackInto(const Pack<T> &rules, const Packet &packet, T &value) {
  PacketReader reader{&packet};
  return rules.unpack(reader, value);
}

/****
 * Pack<> implementations.
 */

/**
 * Pack implementations are transitive, given the existence of mutual
 * assignment operations.
 *
 * This is mainly useful for small types like Clamped and such.
 */
template<typename A, typename B>
struct AssignPack: Pack<B> {
  AssignPack(const Pack<A> &packer) : Pack<B>(
      [packer](PacketWriter &writer, const B &value) {
        A aValue = value;
        packer.pack(writer, aValue);
      },
      [packer](PacketReader &reader, B &value) {
        A aValue;
        if (packer.unpack(reader, aValue)) {
          value = aValue;
          return true;
        }
        return false;
      }) { }
};

/**
 * Pack a boolean in one bit. Doesn't get more space-efficient than this.
 */
struct BoolPack: Pack<bool> {
  BoolPack();
};

/**
 * Pack an enum using a variable number of bits. Don't take more than you use.
 */
template<typename Enum>
struct EnumPack: Pack<Enum> {
  EnumPack<Enum>(int bits) : Pack<Enum>(
      [bits](PacketWriter &writer, const Enum &value) {
        for (unsigned char i = 0; i < bits; i++)
          writer.writeBit((const bool) ((((unsigned char) value) >> i) & 1));
      },
      [bits](PacketReader &reader, Enum &value) {
        unsigned char newValue = 0;
        optional<bool> readBit;
        for (unsigned char i = 0; i < bits; i++) {
          readBit = reader.readBit();
          if (!readBit) return false;
          if (*readBit)
            newValue = (unsigned char) (newValue | (1 << i));
        }
        value = static_cast<Enum>(newValue);
        return true;
      }) { }
};

/**
 * Pack a T byte-wise. Unions to the rescue!
 */
template<typename T>
struct BytePack: Pack<T> {
  BytePack() : Pack<T>(
      [](PacketWriter &writer, const T &value) {
        writer.writeValue<T>(value);
      },
      [](PacketReader &reader, T &value) -> bool {
        optional<T> readValue = reader.readValue<T>();
        if (readValue) value = *readValue;
        else return false;
        return true;
      }) { }
};

/**
 * Pack a std::string, null-terminating.
 */
struct StringPack: Pack<std::string> {
  StringPack();
};

/**
 * Packs an optional<>.
 */
template<typename T>
struct OptionalPack: Pack<optional<T>> {
  OptionalPack(const Pack<T> &rule) : Pack<optional<T>>(
      [rule](PacketWriter &writer, const optional<T> &value) {
        writer.writeBit((bool) value);
        if (value) rule.pack(writer, *value);
      },
      [rule](PacketReader &reader, optional<T> &value) -> bool {
        optional<bool> readBit = reader.readBit();
        if (!readBit) return false;
        if (*readBit) {
          value.emplace(T());
          if (!rule.unpack(reader, *value)) return false;
        } else value.reset();
        return true;
      }) { }
};

/**
 * Packs a std::map<K, V>.
 */
template<typename K, typename V>
struct MapPack: Pack<std::map<K, V>> {
  MapPack(const Pack<K> &keyRule, const Pack<V> &valueRule) :
      Pack<std::map<K, V>>(
          [keyRule, valueRule]
              (PacketWriter &writer, const std::map<K, V> &map) {
            for (auto &pair : map) {
              writer.writeBit(true);
              keyRule.pack(writer, pair.first);
              valueRule.pack(writer, pair.second);
            }
            writer.writeBit(false);
          },
          [keyRule, valueRule]
              (PacketReader &reader, std::map<K, V> &map) -> bool {
            K key;
            V value;
            map.clear();
            optional<bool> readBit = reader.readBit();
            if (!readBit) return false;
            while (*readBit) {
              keyRule.unpack(reader, key);
              if (!valueRule.unpack(reader, value)) return false;
              map.emplace(key, value);
              readBit = reader.readBit();
              if (!readBit) return false;
            }
            return true;
          }) { }
};

/**
 * Pack a std::list's and std::vector's, using a certain size type.
 */
template<typename Listlike, typename T, typename SizeT = size_t>
struct ListlikePack: Pack<Listlike> {
  ListlikePack(const Pack<T> &rule) :
      Pack<Listlike>(
          [rule](PacketWriter &writer, const Listlike &list) {
            writer.writeValue<SizeT>(list.size());
            for (auto &value : list)
              rule.pack(writer, value);
          },
          [rule](PacketReader &reader, Listlike &list) -> bool {
            list.clear();

            optional<SizeT> size = reader.readValue<SizeT>();
            if (!size) return false;
            SizeT i = *size;

            T value;
            while (i > 0) {
              i--;
              if (!rule.unpack(reader, value)) return false;
              list.push_back(value);
            }
            return true;
          }
      ) { }
};

template<typename T, typename SizeT = size_t>
using ListPack = ListlikePack<std::list<T>, T, SizeT>;

template<typename T, typename SizeT = size_t>
using VectorPack = ListlikePack<std::vector<T>, T, SizeT>;

/**
 * Pack a class, holding a variadic argument list of member rules.
 */
template<typename Class, typename Member>
struct MemberRule {
  MemberRule(const Pack<Member> &rule, Member Class::* const ptr) :
      rule(rule), ptr(ptr) { }

  const Pack<Member> rule;
  Member Class::* const ptr;
};

template<typename Class>
struct ClassPack: Pack<Class> {
 private:
  static void classPack(PacketWriter &, const Class &) { }

  template<typename Member, typename... Rest>
  static void classPack(PacketWriter &writer, const Class &value,
                        const MemberRule<Class, Member> &rule, Rest... rest) {
    rule.rule.pack(writer, value.*rule.ptr);
    classPack(writer, value, rest...);
  };

  static bool classUnpack(PacketReader &, Class &) {
    return true;
  }

  template<typename Member, typename... Rest>
  static bool classUnpack(PacketReader &reader, Class &value,
                          const MemberRule<Class, Member> &rule, Rest... rest) {
    if (!rule.rule.unpack(reader, value.*rule.ptr)) return false;
    return classUnpack(reader, value, rest...);
  };

 public:
  template<typename... Members>
  ClassPack(Members... members) : Pack<Class>(
      [members...](PacketWriter &writer, const Class &value) {
        ClassPack::classPack(writer, value, members...);
      },
      [members...](PacketReader &reader, Class &value) -> bool {
        return ClassPack::classUnpack(reader, value, members...);
      }) { }
};

/**
 * Pack a std::pair.
 */

template<typename First, typename Second>
struct PairPack: public Pack<std::pair<First, Second>> {
  PairPack(const Pack<First> &firstRule,
           const Pack<Second> &secondRule) :
      Pack<std::pair<First, Second>>(
          [firstRule, secondRule]
              (PacketWriter &writer, const std::pair<First, Second> &pair) {
            firstRule.pack(writer, pair.first);
            secondRule.pack(writer, pair.second);
          },
          [firstRule, secondRule]
              (PacketReader &reader, std::pair<First, Second> &pair) -> bool {
            if (!firstRule.unpack(reader, pair.first)) return false;
            if (!secondRule.unpack(reader, pair.second)) return false;
            return true;
          }
      ) { }
};

/**
 * Instantiated Pack's.
 */

const static Pack<std::string> stringPack = StringPack();

const static Pack<optional<std::string>> optStringPack =
    OptionalPack<std::string>(stringPack);

const static Pack<bool> boolPack = BoolPack();

const static Pack<float> floatPack = BytePack<float>();

const static Pack<int> intPack = BytePack<int>();

const static Pack<sf::Vector2f> vectorPack =
    ClassPack<sf::Vector2f>(
        MemberRule<sf::Vector2f, float>(floatPack, &sf::Vector2f::x),
        MemberRule<sf::Vector2f, float>(floatPack, &sf::Vector2f::y)
    );

const static Pack<Clamped> clampedPack = AssignPack<float, Clamped>(floatPack);

const static Pack<Angle> anglePack = AssignPack<float, Angle>(floatPack);

const static Pack<Movement> movementPack = EnumPack<Movement>(2);

}

#endif //SOLEMNSKY_PACK_H
