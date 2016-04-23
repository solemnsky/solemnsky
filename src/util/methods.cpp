#define _USE_MATH_DEFINES // for M_PI

#include <sstream>
#include "printer.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <cmath>
#include "methods.h"

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
}

enum_error::enum_error() :
    std::logic_error("An enum is outside its identified value space. Maybe "
                         "you're connection to a corrupted server? Report this "
                         "issue to the solemnsky team along with all the "
                         "relevant logs you can find, it's a bug.") { }

