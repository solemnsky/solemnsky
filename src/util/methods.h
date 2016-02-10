/**
 * Utilities in the form of methods; useful for definitions.
 */
#ifndef SOLEMNSKY_METHODS_H
#define SOLEMNSKY_METHODS_H

#include <SFML/System.hpp>
#include <Box2D/Box2D.h>
#include <functional>

/****
 * Canonical log messages with some handy formatting.
 */

enum class LogOrigin {
  None, // not specified
  Engine, // something deep in the engine
  Network, // transmission stuff
  App, // multimedia management for clients
  Client, // misc. things for clients
  Server, // misc. things for servers
  Error // is a fatal error
};

/**
 * Logging / error throwing functions.
 */
void appLog(const std::string &contents, const LogOrigin = LogOrigin::None);
void appErrorLogic(const std::string &contents); // log and throw
void appErrorRuntime(const std::string &contents); // log and throw

/**
 * Logging for memory leaks.
 */

#define CTOR_LOG(str) appLog("CTOR -> " + (std::string) str)
#define DTOR_LOG(str) appLog("DTOR <- " + (std::string) str)

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

#endif //SOLEMNSKY_METHODS_H
