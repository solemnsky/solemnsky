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
 * Certify that a bunch of optional fields are instantiated and have valid
 * values.
 */
bool verifyFields();

template<typename Field, typename... Fields>
bool verifyFields(Field &field, Fields... fields) {
  if (!field) return false;
  if (!verifyValue(*field)) return false;
  return verifyFields(fields...);
}

/**
 * Random stuff.
 */

std::string inQuotes(const std::string &str);
PID smallestUnused(std::vector<PID> &vec);

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

inline bool imply(const bool x, const bool y) { return y or !x; }

template<typename K, typename V>
V *findValue(std::map<K, V> &m, const K k) {
  auto x = m.find(k);
  if (x != m.end()) return &x->second;
  return nullptr;
}

