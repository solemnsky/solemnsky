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
#include <ratio>
#include <cmath>
#include <numeric>
#include <SFML/System.hpp>
#include <boost/optional.hpp>

/**
 * Small comforts.
 */
struct Nothing { };
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
}

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>

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
 * constricting the way in which it can be used.
 */

/**
 * Floats that are used for a 'cooldown' effect.
 * The 'period' value isn't really part of the state; it's only there to help
 * regulate its usage. Therefore, it isn't synced over the network.
 * (Ideally it should be value templated).
 */
struct Cooldown {
  float cooldown;
  float period;

  Cooldown(const float period);
  bool cool(const float delta);
  void reset();
  void prime();

  inline operator bool() { return cooldown == 0; }

  template<typename Archive>
  void serialize(Archive ar) {
    ar(cooldown);
  }
};

/**
 * Float in the [0, 1] range.
 */
struct Clamped {
 private:
  float value;

 public:
  Clamped();
  Clamped(const float value);
  Clamped &operator=(const float x);
  Clamped &operator+=(const float x);
  Clamped &operator-=(const float x);

  template<typename Archive>
  void serialize(Archive &ar) { ar(value); }

  inline operator float() const { return value; }
};

/**
 * Floats that always cycle back to a certain range when assigned. ([min, max[)
 */
struct Cyclic {
 private:
  friend class Angle;

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
float cyclicDistance(const Cyclic x, const float y);

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
 * The concept of a type that can verify some invariant that could be
 * violated when it's transmitted over the network.
 */
class VerifyStructure {
 public:
  virtual bool verifyStructure() const = 0;
};

/**
 * The concept of a type that can be synced over the network.
 */
template<typename Init, typename Delta>
class Networked {
 public:
  // you are required to construct a Networked object with its
  // initializer type; this supports an orthogonal interface
  Networked() = delete;
  Networked(const Init &) { }
  virtual void applyDelta(const Delta &) = 0;
  virtual Init captureInitializer() const = 0;
};

/**
 * Types and type synonyms for the game.
 */
typedef unsigned int PID; // ID type for various things in the game
typedef unsigned short Port; // network port

namespace sky {

typedef unsigned char Team; // 0 is spectator, 1 left, 2 right

enum class ArenaMode {
  Lobby, // lobby, to make teams
  Game, // playing tutorial
  Scoring // viewing tutorial results
};

}
