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
#include "util/methods.h"
#include <fstream>
#include <list>
#include <polypartition.h>

namespace sky {

/**
 * MapObstacle.
 */

MapObstacle::MapObstacle() { }

MapObstacle::MapObstacle(const sf::Vector2f &pos,
                         const std::vector<sf::Vector2f> &localVerticies,
                         const float damage) :
    pos(pos), localVertices(localVerticies), decomposed(), damage(damage) {

  std::vector<sf::Vector2f> verts(localVerticies);
  pp::Poly poly(verts.data(), verts.size());
  std::list<pp::Poly> tmp;
  pp::Partition part;
  part.ConvexPartition_HM(&poly, &tmp);

  for(auto p : tmp){
    decomposed.push_back(
            std::vector<sf::Vector2f>(p.GetPoints(), p.GetPoints() + p.GetNumPoints()));
  }

}

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
    dimensions(3200, 900),
    name(name) {
  if (name == "test1") {
    loadTest1();
  } else if (name == "test2") {
    loadTest2();
  } else if (name != ""){
    auto file = std::ifstream(rootPath() + "maps/" + name + ".json");
    cereal::JSONInputArchive archive(file);
    archive( cereal::make_nvp("dimensions",dimensions),
             cereal::make_nvp("obstacles",obstacles) );
  }
}

const sf::Vector2f &Map::getDimensions() const {
  return dimensions;
}

const std::vector<MapObstacle> &Map::getObstacles() const {
  return obstacles;
}

const std::vector<MapItem> &Map::getItems() const {
  return items;
}

void Map::save(std::ostream& s) {
  cereal::JSONOutputArchive archive(s);
  archive( cereal::make_nvp("dimensions",dimensions),
           cereal::make_nvp("obstacles",obstacles) );
}

}
