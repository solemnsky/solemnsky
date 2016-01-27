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
 *
 * We don't necessarily translate *all of T* into the Packet when we pack, or
 * overwrite *all of T* from the Packet when we unpack. We merely contend to
 * pull some data from one T and push it into another T, and within certain
 * conditions assume they are in some manner equal.
 *
 * For our particular use-case this turns out to be the practical design.
 */

template<typename T>
struct PackRule {
  PackRule(const std::function<void(Packet &, const T)> &pack,
           const std::function<void(Packet &, T &)> &unpack)
      : pack(pack), unpack(unpack) { }

private:
  template<typename G> friend
  Packet pack(const PackRule<G> &rules, const G val);

  template<typename G> friend
  Packet packInto(const PackRule<G> &rules, Packet &packet, const G val);

  template<typename G> friend
  G unpack(const PackRule<G> &rules, Packet &packet);

  template<typename G> friend
  G unpackInto(const PackRule<G> &rules, Packet &packet, G &val);

  const std::function<void(Packet &, const T)> pack;
  const std::function<void(Packet &, T &)> unpack;
};

template<typename T>
inline Packet pack(const PackRule<T> &rules, const T val) {
  Packet packet;
  rules.pack(packet, val);
  return packet;
}

template<typename T>
inline Packet packInto(const PackRule<T> &rules, Packet &packet, const T val) {
  rules.pack(packet, val);
}

template<typename T>
inline T unpack(const PackRule<T> &rules, Packet &packet) {
  T val;
  rules.unpack(packet, val);
  return val;
}

template<typename T>
inline T unpackInto(const PackRule<T> &rules, Packet &packet, T &val) {
  rules.unpack(packet, val);
  return val;
}

/****
 * Predefined PackRules.
 */

/**
 * ValueRule: packing an intrinsic datatype.
 */
template<typename Value>
class ValueRule {
  static void valuePack(Packet &packet, const Value val) {
    packet.packValue<Value>(val);
  }

  static void valueUnpack(Packet &packet, Value &val) {
    val = packet.unpackValue<Value>();
  }

public:
  static PackRule<Value> rules() {
    return PackRule<Value>(ValueRule<Value>::valuePack,
                           ValueRule<Value>::valueUnpack);
  }
};

/**
 * OptionalRule: packing an optional<>.
 */
template<typename Value>
class OptionalRule {
  static void optPack(const PackRule<Value> &underlying,
                      Packet &packet, const optional<Value> val) {
    if (val) {
      packet.packChar('y');
      packInto(underlying, packet, *val);
    } else packet.packChar('n');
  }

  static void optUnpack(const PackRule<Value> &underlying,
                        Packet &packet,
                        optional<Value> &val) {
    char c = packet.unpackChar();
    if (c == 'y') val.emplace(unpack(underlying, packet));
    else val.reset();
  }

public:
  static PackRule<optional<Value>> rules(const PackRule<Value> &underlying) {
    return PackRule<optional<Value>>(
        [&](Packet &packet, const optional<Value> val) {
          OptionalRule<Value>::optPack(underlying, packet, val);
        },
        [&](Packet &packet, optional<Value> &val) {
          OptionalRule<Value>::optUnpack(underlying, packet, val);
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
             Member Parent::* pointer) :
      packRule(packRule), pointer(pointer) { }

  PackRule<Member> packRule;
  Member Parent::* pointer;
};

template<typename Parent>
class ClassRule {
  static void classPack(Packet &packet, const Parent parent) { }

  template<typename HeadMember, typename... TailMembers>
  static void classPack(Packet &packet, const Parent parent,
                        const MemberRule<Parent, HeadMember> &rule,
                        TailMembers... tailMembers) {
    packInto(rule.packRule, packet, parent.*rule.pointer);
    classPack(packet, parent, tailMembers...);
  }

  static void classUnpack(Packet &packet, Parent &parent) { }

  template<typename HeadMember, typename... TailMembers>
  static void classUnpack(Packet &packet, Parent &parent,
                          const MemberRule<Parent, HeadMember> &rule,
                          TailMembers... tailMembers) {
    parent.*rule.pointer = unpack(rule.packRule, packet);
    classUnpack(packet, parent, tailMembers...);
  }

public:
  template<typename... Members>
  static PackRule<Parent> rules(Members... members) {
    return PackRule<Parent>(
        [&](Packet &packet, const Parent parent) {
          ClassRule<Parent>::classPack(packet, parent, members...);
        },
        [&](Packet &packet, Parent &parent) {
          ClassRule<Parent>::classUnpack(packet, parent, members...);
        }
    );
  }
};

#endif //SOLEMNSKY_PACKER_H
