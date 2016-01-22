/**
 * Maps; physical environment and graphics information.
 */
#ifndef SOLEMNSKY_MAP_H
#define SOLEMNSKY_MAP_H

#include <SFML/System.hpp>
#include <string>

namespace sky {

struct Map {
  Map() : dimensions(3200, 900) { }

  sf::Vector2f dimensions;

  void loadFromFile(std::string filepath) { }
};

}

#endif //SOLEMNSKY_MAP_H
