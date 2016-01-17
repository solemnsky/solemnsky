#include <SFML/Window/Event.hpp>
#include <ntgdi.h>
#include <winddi.h>
#include "sysutil.h"
#include "util.h"

optional<bool> getMouseButtonAction(sf::Event event) {
  if (event.type == sf::Event::MouseButtonReleased)
    return {false};
  if (event.type == sf::Event::MouseButtonPressed)
    return {true};
  return {};
}

sf::Color mixColors(const sf::Color color1, const sf::Color color2,
                    const float degree) {
  return sf::Color(mix(color1.r, color2.r, degree),
                   mix(color1.g, color2.g, degree),
                   mix(color1.b, color2.b, degree),
                   mix(color1.a, color2.a, degree)
  );
}