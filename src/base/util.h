/**
 * Generally helpful utilities.
 */
#ifndef SOLEMNSKY_BASE_UTIL_H
#define SOLEMNSKY_BASE_UTIL_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <functional>

#include <experimental/optional> // for use downstream

using std::experimental::optional;

enum class LogType {
  Info, // you might like to know it
  Notice, // you should know it
  Error // you have to know it (buzzfeed articles, etc)
};

void app_log(LogType type, const std::string contents);

/**
 * Manages an action that becomes momentarily active on regular intervals.
 */
class Ticker {
private:
  bool active = false;

public:
  float cooldown;
  float period;

  Ticker(float period) : period(period), cooldown(period) { }

  bool tick(float delta);
  void prime();
  void wait();

  operator bool();
};

/**
 * Convert between box2d and SFML types.
 */
class FromBox2d {
public:
  static sf::Vector2f vector(b2Vec2 vec);
};

#endif //SOLEMNSKY_BASE_UTIL_H
