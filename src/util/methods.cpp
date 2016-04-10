#include <sstream>
#include <vector>

#define _USE_MATH_DEFINES // for M_PI

#include <math.h>
#include <iostream>
#include <cmath>
#include "methods.h"
#include "types.h"

std::string showTime() {
  static sf::Clock clock;
  std::string clockTime =
      std::to_string(clock.getElapsedTime().asMilliseconds());
  return clockTime + std::string(10 - clockTime.size(), ' ');
}

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

bool verifyFields() {
  return true;
}

std::string inQuotes(const std::string &str) {
  return "\"" + str + "\"";
}

int smallestUnused(std::vector<int> &vec) {
  // TODO: this could be faster
  std::sort(vec.begin(), vec.end());
  int v = 0;
  for (const int x : vec) {
    if (x == v) {
      v++;
      continue;
    }
    break;
  }
  return v;
};


