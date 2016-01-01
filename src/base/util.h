/**
 * Generally helpful utilities. If any of this starts getting large, it'll be
 * separated into another file.
 */
#ifndef SOLEMNSKY_BASE_UTIL_H
#define SOLEMNSKY_BASE_UTIL_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <functional>

/****
 * Optional.
 */

// TODO: use Boost's optional instead of this
template<typename T>
class Optional {
private:
  T *value;
  bool has{false};

public:
  Optional() { }

  Optional(T value) {
    this->value = &value;
    has = true;
  }

  Optional(const Optional<T> &x) {
    this->has = x.has;
    if (has) this->value = x.value;
  }

  T operator*() {
    assert(has);
    return *value;
  }

  T *operator->() {
    assert(has);
    return value;
  }

  operator bool() { return has; }

  Optional<T> &operator=(Optional<T> &&x) {
    this->has = x.has;
    if (has) this->value = x.value;
    return *this;
  }
};

/****
 * Canonical log messages with some handy formatting.
 */

enum class LogType {
  Info, // you might like to know it
  Notice, // you should know it
  Error // you have to know it (buzzfeed articles, etc)
};

/**
 * Logs a pretty little message.
 */
void app_log(LogType type, const std::string contents);

/****
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

/****
 * Extract useful information from sf::Events.
 */

Optional<bool> getMouseButtonAction(sf::Event event);

#endif //SOLEMNSKY_BASE_UTIL_H
