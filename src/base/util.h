/**
 * Generally helpful utilities. If any of this starts getting large, it'll be
 * separated into another file.
 */
#ifndef SOLEMNSKY_BASE_UTIL_H
#define SOLEMNSKY_BASE_UTIL_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <functional>
#include <boost/optional.hpp>

/****
 * Optional.
 */

using boost::optional;

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
void appLog(LogType type, const std::string contents);

/****
 * Extract useful information from sf::Events.
 */

optional<bool> getMouseButtonAction(sf::Event event);

/****
 * Clamp values
 */

template<typename T>
T clamp(const T min, const T max, const T x) {
  if (x > max) return max;
  if (x < min) return min;
  return x;
}

/****
 * Do stuff with colors.
 */
sf::Color mixColors(
    const sf::Color color1, const sf::Color color2, const float degree);

/****
 * For some reason sf::Vector has no math utilities, here are some.
 */
class VecMath {
public:
  static float length(const sf::Vector2f &vec);
  static sf::Vector2f fromAngle(const float angle);
  static float angle(const sf::Vector2f &vec);
};


#endif //SOLEMNSKY_BASE_UTIL_H
