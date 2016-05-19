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
 * Utilities in the form of methods; useful for definitions.
 */
#pragma once
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>
#include <functional>
#include "types.h"
#include <cereal/archives/json.hpp>

// log and throw
// log and throw

/**
 * Logging for memory leaks.
 */

//#define CTOR_LOG(str) appLog("CTOR --> " + (std::string) str)
//#define DTOR_LOG(str) appLog("DTOR <- " + (std::string) str)
#define DTOR_LOG(str) {}while(0)
#define CTOR_LOG(str) {}while(0)

/****
 * For some reason sf::Vector has no math utilities, here are some.
 */
class VecMath {
 public:
  static float length(const sf::Vector2f &vec);
  static sf::Vector2f fromAngle(const float angle);
  static float angle(const sf::Vector2f &vec);
};

float toRad(const float x);
float toDeg(const float x);

/**
 * Tweening and animation.
 */

float linearTween(const float begin, const float end, const float time);
float sineAnim(const float time, const float period);

/**
 * Verify that a value respects any potential invariants.
 */
template<typename X>
bool verifyValue(
    const X &x,
    typename std::enable_if<std::is_base_of<
        VerifyStructure, X>::value>::type * = 0) {
  return x.verifyStructure();
}

template<typename X>
bool verifyValue(
    const X &x,
    typename std::enable_if<!std::is_base_of<
        VerifyStructure, X>::value>::type * = 0) {
  return true;
}

/**
 * Verify that a series of optionals are instantiated, and
 * that the values respect any potential invariants.
 */
bool verifyOptionals();

template<typename Field, typename... Fields>
bool verifyOptionals(Field &field, Fields... fields) {
  if (!bool(field)) return false;
  if (!verifyValue(*field)) return false;
  return verifyOptionals(fields...);
}

/**
 * Verify that the values of a map respect any potential invariants.
 */
template<typename Key, typename Value>
bool verifyMap(const std::map<Key, Value> &map) {
  for (const auto &x : map)
    if (!verifyValue(x.second)) return false;
  return true;
}

/**
 * Verify that the values of a vector respect any potential invariants.
 */
template<typename Value>
bool verifyVector(const std::vector<Value> &vec) {
  for (const auto &x : vec)
    if (!verifyValue(x)) return false;
  return true;
}

/**
 * Random stuff.
 */

std::string inQuotes(const std::string &str);
PID smallestUnused(std::vector<PID> &vec);

template<typename T>
PID smallestUnused(const std::map<PID, T> &map) {
  // this could be faster, but it really doesn't need to be
  // don't waste your effort here, waste it in other places!
  PID i{0};
  for (const auto &pair: map) {
    if (pair.first == i) ++i;
    else break;
  }
  return i;
}

class enum_error: public std::logic_error {
 public:
  enum_error();
};

inline bool imply(const bool x, const bool y) { return y or !x; }

inline const std::string rootPath() { return "../../"; }
