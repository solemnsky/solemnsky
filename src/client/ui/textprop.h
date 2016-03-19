/**
 * Properties used when drawing text.
 */
#pragma once
#include <SFML/Graphics.hpp>
#include "client/util/resources.h"

namespace ui {

struct TextProperties {
  static const TextProperties normal;

  int size = 24;
  sf::Color color = sf::Color::White;
  int maxWidth = 0;
  bool alignBottom = false;
  sf::Font const *font = &fontOf(Res::Font);
  sf::Text::Style style = sf::Text::Regular;
};

}
