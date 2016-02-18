/**
 * Maps; physical environment and graphics information.
 */
#ifndef SOLEMNSKY_MAP_H
#define SOLEMNSKY_MAP_H

#include <SFML/System.hpp>
#include <string>

namespace sky {

using MapName = std::string;

struct Map {
  Map(const MapName &name);

  sf::Vector2f dimensions;
};

}

#endif //SOLEMNSKY_MAP_H
