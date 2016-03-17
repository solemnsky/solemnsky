/**
 * The static environment in which a Sky plays out.
 */
#pragma once
#include <SFML/System.hpp>
#include <string>

namespace sky {

using MapName = std::string;

struct Map {
  Map(const MapName &name);

  // stub: awaiting broad design decisions

  sf::Vector2f dimensions;
};

}
