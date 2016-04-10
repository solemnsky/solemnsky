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
 * Tweening.
 */

float linearTween(const float begin, const float end, const float time);

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
};

/**
 * Random stuff.
 */

std::string inQuotes(const std::string &str);
int smallestUnused(std::vector<int> &vec);
