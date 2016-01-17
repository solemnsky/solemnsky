#include <SFML/Window/Event.hpp>
#include "sysutil.h"
#include "util.h"

optional<bool> getMouseButtonAction(sf::Event event) {
  if (event.type == sf::Event::MouseButtonReleased)
    return {false};
  if (event.type == sf::Event::MouseButtonPressed)
    return {true};
  return {};
}

inline uint8 mix(const uint8 x1, const uint8 x2, const float degree) {
  const float _x1(x1), _x2(x2);
  return (uint8) (std::round(degree * (_x2 - _x1)) + _x1);
}

sf::Color mixColors(const sf::Color color1, const sf::Color color2,
                    const float degree) {
  return sf::Color(mix(color1.r, color2.r, degree),
                   mix(color1.g, color2.g, degree),
                   mix(color1.b, color2.b, degree),
                   mix(color1.a, color2.a, degree)
  );
}