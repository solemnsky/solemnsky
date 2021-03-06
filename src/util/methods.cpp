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
#define _USE_MATH_DEFINES // for M_PI

#include <sstream>
#include <cmath>
#include "printer.hpp"
#include "methods.hpp"

/**
 * VecMath.
 */
float VecMath::length(const sf::Vector2f &vec) {
  return (float) sqrt(vec.x * vec.x + vec.y * vec.y);
}

sf::Vector2f VecMath::fromAngle(const float angle) {
  const float rad = toRad(angle);
  return sf::Vector2f(std::cos(rad), std::sin(rad));
}

float VecMath::angle(const sf::Vector2f &vec) {
  return toDeg(std::atan2(vec.y, vec.x));
}

float toDeg(const float x) {
  constexpr float factor = (float) (180 / M_PI);
  return x * factor;
}

float toRad(const float x) {
  constexpr float factor = (float) (M_PI / 180);
  return x * factor;
}

/**
 * Tweening.
 */

float linearTween(const float begin, const float end, const float time) {
  return clamp<float>(std::min(begin, end),
                      std::max(begin, end), begin + time * (end - begin));
}

float sineAnim(const float time, const float period) {
  return (1.0f + float(sin((time * M_2_PI) / period))) / 2.0f;
}

bool verifyRequiredOptionals() {
  return true;
}

bool verifyOptionals() {
  return true;
}

PID smallestUnused(std::vector<PID> &vec) {
  // this could be faster, but it really doesn't need to be
  // don't waste your effort here, waste it in other places!
  std::sort(vec.begin(), vec.end());
  PID v{0};
  for (const auto x : vec) {
    if (x == v) ++v;
    else break;
  }
  return v;
}

/**
 * enum_error, because C++ doesn't provide guarantees about enum ranges.
 */
enum_error::enum_error() :
    std::logic_error(
        "An enum is outside its identified value space. Perhaps "
            "you're connected to a corrupted server? Report this "
            "issue to the solemnsky team along with all the "
            "relevant logs you can find.") { }
bool imply(const bool x, const bool y) {
  return y or !x;
}

/**
 * Platform agnostic system stuff.
 */

int getProcessID() {
#if defined(__linux) || defined(__APPLE__)
#include <unistd.h>
  return int(getpid());
#endif

#ifdef _WIN32
  return int(GetCurrentProcessId());
#endif
}

void runSystemQuiet(const std::string &command) {
#if defined(__linux) || defined(__APPLE__)
  system((command + " > /dev/null 2>&1").c_str());
#endif

#ifdef _WIN32
  system((command + " > NUL").c_str());
#endif
}

