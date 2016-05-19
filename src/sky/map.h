/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * The *static* aspect of the environment of a Sky.
 */
#pragma once
#include <SFML/System.hpp>
#include "util/types.h"
#include <string>

namespace sky {

/**
 * Map name, the handle people should know the map by.
 */
using MapName = std::string;

/**
 * A shape that things can collide with.
 */
struct MapObstacle {
  MapObstacle();
  MapObstacle(const sf::Vector2f &pos,
              const std::vector<sf::Vector2f> &localVerticies,
              const float damage);

  sf::Vector2f pos;
  std::vector<sf::Vector2f> localVerticies;
  float damage;
};

struct MapItem {
  enum class Type {
    RedThingy, BlueThingy
  } type;
  sf::Vector2f pos;
};

/**
 * A whole static environment for a Sky.
 */
struct Map {
 private:
  // Hard-coded maps for now.
  void loadTest1();
  void loadTest2();

  // State, all constant after loading.
  sf::Vector2f dimensions;
  std::vector<MapObstacle> obstacles;
  std::vector<MapItem> items;

 public:
  Map(const MapName &name);

  const MapName name;
  const sf::Vector2f &getDimensions() const;
  const std::vector<MapObstacle> &getObstacles() const;
  const std::vector<MapItem> &getItems() const;

};

}
