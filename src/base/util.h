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
  Error, // you have to know it (buzzfeed articles, etc)
  Debug // exists for debugging
};

/**
 * Logs a pretty little message.
 */
void appLog(LogType type, const std::string contents);

/**
 * Logging that can be conditionally included.
 */

#define DEBUG_LOG(str) appLog(LogType::Debug, str)
#define LIFE_LOG(str) appLog(LogType::Debug, str)

/****
 * Extract useful information from sf::Events.
 */

optional<bool> getMouseButtonAction(sf::Event event);

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

float toRad(const float x);

float toDeg(const float x);


#endif //SOLEMNSKY_BASE_UTIL_H
