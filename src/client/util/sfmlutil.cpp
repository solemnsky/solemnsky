#include "sfmlutil.h"

inline sf::Uint8 mix(const sf::Uint8 x1,
                     const sf::Uint8 x2,
                     const float degree) {
  const float _x1(x1), _x2(x2);
  return (sf::Uint8) (std::round(degree * (_x2 - _x1)) + _x1);
}

sf::Color mixColors(const sf::Color color1, const sf::Color color2,
                    const float degree) {
  return sf::Color(mix(color1.r, color2.r, degree),
                   mix(color1.g, color2.g, degree),
                   mix(color1.b, color2.b, degree),
                   mix(color1.a, color2.a, degree)
  );
}