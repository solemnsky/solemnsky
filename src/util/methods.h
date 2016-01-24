/**
 * Utilities in the form of methods; useful for definitions.
 */
#ifndef SOLEMNSKY_SYSUTIL_H
#define SOLEMNSKY_SYSUTIL_H

#include <SFML/System.hpp>
#include <Box2D/Box2D.h>
#include <functional>

/****
 * Canonical log messages with some handy formatting.
 */

enum class LogType {
  Info, // you might like to know it
  Notice, // you should know it
  Error, // you have to know it (buzzfeed articles, etc)
  Debug // exists for debugging
};

/**
 * Logs a pretty little message.
 */
void appLog(LogType type, const std::string contents);

/**
 * Logging for memory leaks.
 */

#define CTOR_LOG(str) appLog(LogType::Debug, "CTOR -> " + (std::string) str)
#define DTOR_LOG(str) appLog(LogType::Debug, "DTOR <- " + (std::string) str)

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

#endif //SOLEMNSKY_SYSUTIL_H
