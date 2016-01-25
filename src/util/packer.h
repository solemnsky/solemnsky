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
 * PackRule<T>: the packing / unpacking of a T.
 */

template<typename T>
struct PackRule {
  PackRule(const std::function<void(Packet &, const T)> &pack,
           const std::function<T(Packet &)> unpack)
      : pack(pack), unpack(unpack) { }

  const std::function<void(Packet &, const T)> pack;
  const std::function<T(Packet &)> unpack;
};

template<typename T>
Packet pack(const PackRule<T> &rules, const T x) {
  Packet packet;
  rules.pack(packet, x);
  return packet;
}

template<typename T>
T unpack(const PackRule<T> &rules, Packet &packet) {
  return rules.unpack(packet);
}

/****
 * Predefined PackRules.
 */

/**
 * ValueRule: packing an intrinsic datatype.
 */
template<typename Value>
class ValueRule {
  static void pack(Packet &packet, const Value val) {
    packet.packValue<Value>(val);
  }

  static Value unpack(Packet &packet) {
    return packet.unpackValue<Value>();
  }

public:
  static PackRule<Value> rules() {
    return PackRule<Value>(ValueRule<Value>::pack, ValueRule<Value>::unpack);
  }
};

/**
 * OptionalRule:
 */
template<typename Value>
class OptionalRule {
  static void pack(const PackRule<Value> &underlying,
                   Packet &packet, const optional<Value> x) {
    if (x) {
      packet.packChar('y');
      underlying.pack(packet, *x);
    } else packet.packChar('n');
  }

  static optional<Value> unpack(const PackRule<Value> &underlying,
                                Packet &packet) {
    char c = packet.unpackChar();
    if (c == 'y') return {underlying.unpack(packet)};
    else return {};
  }

public:
  static PackRule<optional<Value>> rules(const PackRule<Value> &underlying) {
    return PackRule<optional<Value>>(
        [&](Packet &packet, const optional<Value> x) {
          OptionalRule<Value>::pack(underlying, packet, x);
        },
        [&](Packet &packet) {
          return OptionalRule<Value>::unpack(underlying, packet);
        }
    );
  }
};

/**
 * ClassRule: rather sickening, but extremely helpful.
 */

template<typename Parent, typename Member>
struct MemberRule {
  MemberRule(const PackRule<Member> &packRule,
             std::function<Member(const Parent)> getter,
             std::function<void(Parent &, Member)> setter) :
      packRule(packRule), getter(getter), setter(setter) { }

  PackRule<Member> packRule;
  std::function<Member(const Parent)> getter;
  std::function<void(Parent &, Member)> setter;
};

template<typename Parent>
class ClassRule {
  static void pack(Packet &packet, const Parent parent) {
    // pack endpoint: do nothing
  }

  template<typename HeadMember, typename... TailMembers>
  static void pack(Packet &packet, const Parent parent,
                   const MemberRule<Parent, HeadMember> &rule,
                   TailMembers... tailMembers) {
    rule.packRule.pack(packet, rule.getter(parent));
    pack(packet, parent, tailMembers...);
  }

  static Parent unpack(Packet &packet, Parent &parent) {
    // unpack endpoint: return the parent
    return parent;
  }

  template<typename HeadMember, typename... TailMembers>
  static Parent unpack(Packet &packet, Parent &parent,
                       const MemberRule<Parent, HeadMember> &rule,
                       TailMembers... tailMembers) {
    rule.setter(parent, rule.packRule.unpack(packet));
    return unpack(packet, parent, tailMembers...);
  }

public:
  template<typename... Members>
  static PackRule<Parent> rules(Members... members) {
    return PackRule<Parent>(
        [&](Packet &packet, const Parent parent) {
          ClassRule<Parent>::pack(packet, parent, members...);
        },
        [&](Packet &packet) {
          Parent parent{};
          return ClassRule<Parent>::unpack(
              packet, parent, members...);
        }
    );
  }
};

#endif //SOLEMNSKY_PACKER_H
