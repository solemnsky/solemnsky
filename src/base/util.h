/**
 * Generally helpful utilities.
 */
#ifndef SOLEMNSKY_BASE_UTIL_H
#define SOLEMNSKY_BASE_UTIL_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <functional>

// TODO: understand the difference between mine and Boost's
template<typename T>
class Optional {
private:
  T value[]{};
  bool has;

public:
  Optional(T value) : has{true} { this->value[0] = value; }

  Optional() : has{false} { };

  T operator*() { return value[0]; }

  T *operator->() { return &value[0]; }

  operator bool() { return has; }
};

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

std::string show(float x);

std::string show(int x);

#endif //SOLEMNSKY_BASE_UTIL_H
