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
#include <cereal/archives/json.hpp>
#include "types.hpp"
#include "util/printer.hpp"
#include <boost/lexical_cast.hpp>

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
 * Tweening, ranges, etc.
 */

float linearTween(const float begin, const float end, const float time);
float sineAnim(const float time, const float period);

template<typename Value>
bool inRange(const Value x, const Value min, const Value max) {
  return (x >= min) && (x <= max);
}

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
    const X &,
    typename std::enable_if<!std::is_base_of<
        VerifyStructure, X>::value>::type * = 0) {
  return true;
}

/**
 * Verify that a series of optionals are instantiated, and
 * that the values respect any potential invariants.
 */
bool verifyRequiredOptionals();

template<typename Field, typename... Fields>
bool verifyRequiredOptionals(const Field &field, Fields... fields) {
  if (!bool(field)) return false;
  if (!verifyValue(*field)) return false;
  return verifyRequiredOptionals(fields...);
}

/**
 * Verify that a series of optionals, if instantiated,
 * respect any potential invariants.
 */
bool verifyOptionals();

template<typename Field, typename... Fields>
bool verifyOptionals(const Field &field, Fields... fields) {
  if (bool(field)) {
    if (!verifyValue(*field)) return false;
  }
  return verifyRequiredOptionals(fields...);
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

/**
 * Allocate the smallest PID that isn't already used in a vector.
 */
PID smallestUnused(std::vector<PID> &vec);

/**
 * Allocate the smallest PID that isn't already used in a map.
 */
template<typename T>
PID smallestUnused(const std::map<PID, T> &map) {
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

/**
 * Boolean implication, i.e. the -> operator.
 */
bool imply(const bool x, const bool y);

/**
 * Return our process' ID.
 */
int getProcessID();

/**
 * Run a command in the system's shell, without printing any output to stdout.
 */
void runSystemQuiet(const std::string &command);

/**
 * Read values from string inputs, wrapper around boost::lexical_cast.
 */
template<typename Value>
optional<Value> readString(const std::string &string) {
  try {
    return boost::lexical_cast<Value>(string);
  } catch (boost::bad_lexical_cast &) {
    return {};
  }
}

inline optional<float> readFloat(const std::string &string) {
  return readString<float>(string);
}

inline optional<double> readDouble(const std::string &string) {
  return readString<double>(string);
}
