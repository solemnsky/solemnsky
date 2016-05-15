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
#include "map.h"

namespace sky {

/**
 * MapObstacle.
 */

MapObstacle::MapObstacle() { }

MapObstacle::MapObstacle(const sf::Vector2f &pos,
                         const std::vector<sf::Vector2f> &localVerticies,
                         const float damage) :
    pos(pos), localVerticies(localVerticies), damage(damage) { }

/**
 * MapItem.
 */

/**
 * Map.
 */

void Map::loadTest1() {
  dimensions = {1600, 900};

  const std::vector<sf::Vector2f> square = {
      {-20, -20}, {-20, 20}, {20, 20}, {20, -20}
  };

  for (float x = 0; x < 1600.0f; x += 200) {
    obstacles.emplace_back(sf::Vector2f(x, 450), square, 1);
  }
}

void Map::loadTest2() {
  dimensions = {3200, 900};
}

Map::Map(const MapName &name) :
    dimensions(3200, 900) {
  if (name == "test1") {
    loadTest1();
  } else if (name == "test2") {
    loadTest2();
  }
}

}
