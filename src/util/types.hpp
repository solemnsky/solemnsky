/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * Utilities in the form of types, useful for declarations.
 */
#pragma once
#include <vector>
#include <numeric>
#include <ratio>
#include <cmath>
#include <numeric>
#include <SFML/System.hpp>
#include <boost/optional.hpp>
#include <SFML/Graphics.hpp>
#include <bitset>

/**
 * The concept of a type that can verify some invariant that could be
 * violated when it's transmitted over the network.
 * TODO: issue #29
 */
class VerifyStructure {
 public:
  virtual bool verifyStructure() const = 0;
  virtual ~VerifyStructure() { }
};

/**
 * This is pretty useful.
 */
using boost::optional;

/**
 * Some helpful units.
 */
using Time = double;
using TimeDiff = float;
using Kbps = float;

/**
 * Cereal rules, in both meanings of the word.
 */
template<typename Archive, typename T>
void save(Archive &ar, const optional<T> &x) {
  if (x) {
    ar(true);
    ar(*x);
  } else ar(false);
}

template<typename Archive, typename T>
void load(Archive &ar, optional<T> &x) {
  bool flag(false);
  ar(flag);
  if (flag) {
    x.emplace();
    ar(*x);
  } else x.reset();
}

#include <cereal/cereal.hpp>

namespace sf {
template<typename Archive>
void serialize(Archive &ar, sf::Vector2f &x) {
  ar(cereal::make_nvp("x", x.x), cereal::make_nvp("y", x.y));
}

template<typename Archive>
void serialize(Archive &ar, sf::Vector2u &x) {
  ar(cereal::make_nvp("x", x.x), cereal::make_nvp("y", x.y));
}

template<typename Archive>
void serialize(Archive &ar, sf::Vector2i &x) {
  ar(cereal::make_nvp("x", x.x), cereal::make_nvp("y", x.y));
}

template<typename Archive>
void serialize(Archive &ar, sf::Color &x) {
  ar(cereal::make_nvp("r", x.r),
     cereal::make_nvp("g", x.g),
     cereal::make_nvp("b", x.b),
     cereal::make_nvp("a", x.a));
}

}

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>

/**
 * The unit type, with exactly one value.
 */
struct Nothing {
  template<typename Archive>
  void serialize(Archive &ar) { }
};

/**
 * Useful functions.
 */
template<typename T>
T clamp(const T min, const T max, const T x) {
  if (x > max) return max;
  if (x < min) return min;
  return x;
}

template<typename T>
T cyclicClamp(const T min, const T max, const T x) {
  if (x > max) return min + std::fmod(x - min, max - min);
  if (x < min) return max - cyclicClamp(min, max, (min - x));
  if (x == max) return min;
  return x;
}

template<typename T>
T sign(const T x) {
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

/**
 * Approach a value at a linear rate, potentially hitting and staying at it.
 */
template<typename T, typename G>
bool approach(T &x, const G target, const G amount) {
  // having a different type variables for the reference is useful when it's
  // one of the containers we define below; approach(Clamped &, const float,
  // const float)  works, for instance.
  const G msign = sign(target - x);
  const G naive = x + msign * amount;
  if (sign(target - naive) != msign) {
    x = target;
    return true;
  }
  x = naive;
  return false;
}

template<typename T>
bool approach(T &x, const T target, const T amount) {
  return approach<T, T>(x, target, amount);
}

/**
 * Maintains a rolling sampling window, which can be queried for statistics.
 * TODO: optimize this if necessary
 */
template<typename Data>
class RollingSampler {
 private:
  std::vector<Data> data;
  const unsigned int maxMemory;

 public:
  RollingSampler() = delete;
  RollingSampler(const unsigned int maxMemory) :
      maxMemory(maxMemory) { }

  void push(const Data value) {
    if (data.size() >= maxMemory) data.erase(data.begin());
    data.push_back(value);
  }

  template<typename Result>
  Result mean() const {
    if (data.size() == 0) return 0;
    return Result(std::accumulate(
        data.begin(), data.end(), Data(0), std::plus<Data>()))
        / Result(data.size());
  }

  Data max() const {
    if (data.size() == 0) return 0;
    return *std::max_element(data.begin(), data.end());
  }

  Data min() const {
    if (data.size() == 0) return 0;
    return *std::min_element(data.begin(), data.end());
  }
};

struct TimeStats {
  TimeStats() = default;
  TimeStats(const RollingSampler<TimeDiff> &sampler);

  TimeDiff min, mean, max;
  std::string print() const;

};

/****
 * Float-augmentation types.
 * Below we have a series of types that build a wrapper around a float,
 * restriction the way in which it can be used.
 */

/**
 * Floats that are used to represent a cooldown, always inside the range [0, 1].
 */
struct Cooldown {
 private:
  float value;

 public:
  Cooldown() : value(1) { }

  explicit Cooldown(const float value) :
      value(value) { }

  inline void reset() {
    value = 1;
  }

  inline void prime() {
    value = 0;
  }

  inline bool cool(const float delta) {
    value = std::max(0.0f, value - delta);
    return value == 0;
  }

  inline operator bool() const {
    return value == 0;
  }

  inline operator float() const {
    return value;
  }

  bool operator==(const Cooldown &x) {
    return x.value == value;
  }

  bool operator!=(const Cooldown &x) {
    return x.value != value;
  }

  template<typename Archive>
  void serialize(Archive ar) {
    ar(value);
  }
};

/**
 * Like a cooldown, but with a variable value range. Whereas Cooldown is used in game state that needs to be
 * synchronized, Scheduler is used to schedule (potentially recurring) actions in the application.
 */
struct Scheduler {
 private:
  Time value, interval;

 public:
  Scheduler() = delete;

  explicit Scheduler(const Time interval) :
      value(interval), interval(interval) { }

  inline bool tick(const TimeDiff delta) {
    value = std::max(0.0, value - Time(delta));
    return value == 0;
  }

  inline operator bool() const {
    return value == 0;
  }

  inline void reset() {
    value = interval;
  }

  inline void prime() {
    value = 0;
  }

};

/**
 * Float in the [0, 1] range.
 */
struct Clamped {
 private:
  float value;

 public:
  Clamped() : value(0) { }

  explicit Clamped(const float value) :
      value(clamp(0.0f, 1.0f, value)) { }

  inline Clamped &operator=(const float x) {
    operator=(Clamped(x));
    return *this;
  }

  inline Clamped &operator+=(const float x) {
    value = clamp(0.0f, 1.0f, value + x);
    return *this;
  }

  inline Clamped &operator-=(const float x) {
    value = clamp(0.0f, 1.0f, value - x);
    return *this;
  }

  inline operator float() const {
    return value;
  }

  bool operator==(const Clamped &x) {
    return x.value == value;
  }

  bool operator!=(const Clamped &x) {
    return x.value != value;
  }

  template<typename Archive>
  void serialize(Archive &ar) { ar(value); }

};

/**
 * Floats that always cycle back to a certain range when assigned. ([min, max[)
 */
struct Cyclic {
 private:
  friend struct Angle;

  float value;
 public:
  float min, max;

  Cyclic() = delete;

  Cyclic(const float min, const float max);
  Cyclic(const float min, const float max, const float value);
  Cyclic &operator=(const float x);
  Cyclic &operator+=(const float x);
  Cyclic &operator-=(const float x);

  inline operator float() const { return value; }
};

bool cyclicApproach(Cyclic &x, const float target, const float amount);
float cyclicDistance(const Cyclic &x, const float y);

/**
 * The linear movement of a one-dimensional variable at a unit or null rate.
 */
enum class Movement {
  Up, Down, None
};

float movementValue(const Movement movement);
Movement addMovement(const bool down, const bool up);

/**
 * Angle value: represents an angle in degrees.
 */
struct Angle {
 private:
  Cyclic value;
 public:
  inline Angle() : Angle(0) { }
  Angle(const float x);
  Angle(const sf::Vector2f &);

  template<typename Archive>
  void serialize(Archive &ar) { ar(cereal::make_nvp("angle", value.value)); }

  Angle &operator=(const float x);
  Angle &operator+=(const float x);
  Angle &operator-=(const float x);

  sf::Vector2f toVector();

  inline operator float() const { return value; }
};

/**
 * Types and type synonyms for the game.
 */
typedef unsigned int PID; // ID type for various things in the game
typedef unsigned short Port; // network port

/**
 * Set of properties indexed by an enum.
 */
template<typename Enum>
struct SwitchSet {
 private:
  std::bitset<size_t(Enum::MAX)> bitset;

 public:
  SwitchSet(const bool init = false) :
      bitset(size_t(init)) { }

  SwitchSet(const Enum key) {
    bitset[size_t(key)] = true;
  }

  SwitchSet operator&&(const SwitchSet &teamSet) {
    bitset &= teamSet.bitset;
    return *this;
  }

  SwitchSet(std::initializer_list<Enum> keys) {
    for (const auto key : keys) bitset[size_t(key)] = true;
  }

  void set(const Enum key, const bool value) {
    bitset.set(size_t(key), value);
  }

  bool get(const Enum key) const {
    return bitset[size_t(key)];
  }

  template<Enum key>
  bool get() const {
    return bitset[size_t(key)];
  }

  bool operator==(const SwitchSet &set) const {
    return bitset == set.bitset;
  }

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    bool x;
    for (size_t i = 0; i < size_t(Enum::MAX); ++i) {
      x = bitset[i];
      ar(x);
      bitset[i] = x;
      // Heh.
    }
  }

};

