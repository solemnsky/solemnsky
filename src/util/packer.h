/**
 * A basic system for packing and unpacking data to and from packets.
 */
#ifndef SOLEMNSKY_PACKER_H
#define SOLEMNSKY_PACKER_H

#include <string>
#include <memory>
#include "util/types.h"
#include "util/methods.h"

/**
 * Packet: the data, with a small interface for reading and writing primitives.
 */
class Packet {
private:
  int readHead = 0;

public:
  Packet();
  Packet(std::vector<char> data);
  Packet(const Packet &packet);
  Packet(Packet &&packet);
  Packet operator=(const Packet &packet);
  Packet operator=(Packet &&packet);

  std::vector<char> data;

  /**
   * Packing and unpacking basic values.
   */
  template<typename T>
  T unpackValue() {
    union {
      char chars[sizeof(T)];
      T value;
    } magic;

    for (size_t i = 0; i < sizeof(T); i++)
      magic.chars[i] = data.at(readHead + i);

    readHead += sizeof(T);
    return magic.value;
  }

  template<typename T>
  void packValue(const T x) {
    union {
      char chars[sizeof(T)];
      T value;
    } magic;

    magic.value = x;

    for (size_t i = 0; i < sizeof(T); i++)
      data.push_back(magic.chars[i]);
  }

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
struct PackRulesAbstract {
  virtual T unpack(Packet &packet) const = 0;
  virtual void pack(Packet &packet, const T &t) const = 0;
};

template<typename T>
struct PackRules {
  std::unique_ptr<PackRulesAbstract<T>> abstract;

  PackRules(std::unique_ptr<PackRulesAbstract<T>> abstract) :
      abstract(std::move(abstract)) {}

  inline T unpack(Packet &packet) const {
    return abstract->unpack(packet);
  }

  void pack(Packet &packet, const T &t) const {
    abstract->pack(packet, t);
  }
};

template<typename T>
Packet pack(const PackRules<T> &rules, T x) {
  Packet packet;
  rules.pack(packet, x);
  return packet;
}

template<typename T>
T unpack(const PackRules<T> &rules, Packet &packet) {
  return rules.unpack(packet);
}

/**
 * Predefined PackRules.
 */
struct PackFloat : public PackRulesAbstract<float> {
  float unpack(Packet &packet) const override;
  void pack(Packet &packet, const float &t) const override;

  static PackRules<float> packer();
};

template<typename T>
struct PackOptional : PackRulesAbstract<optional<T>> {
  std::unique_ptr<PackRulesAbstract<T>> underlyingRules;

  PackOptional<T>(std::unique_ptr<PackRulesAbstract<T>> underlyingRules) :
      underlyingRules(std::move(underlyingRules)) { }

  optional<T> unpack(Packet &packet) const override {
    char c = packet.unpackChar();
    if (c == 'y') return {underlyingRules->unpack(packet)};
    else return {};
  }

  void pack(Packet &packet, const optional<T> &x) const override {
    if (x) {
      packet.packChar('y');
      underlyingRules->pack(packet, x);
    } else packet.packChar('n');
  }

  static PackRules<optional<T>> packer(const PackRules<T> &underlying) {
    return PackRules<optional<T>>(
        std::make_unique<PackOptional<T>>(std::move(underlying.abstract)));
  }
};

#endif //SOLEMNSKY_PACKER_H
