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
#include <fstream>
#include <list>
#include <polypartition.hpp>
#include "map.hpp"
#include "util/methods.hpp"
#include "util/printer.hpp"

namespace sky {

/**
 * SpawnPoint.
 */

sky::SpawnPoint::SpawnPoint(const sf::Vector2f &pos, const Angle &angle) :
    pos(pos), angle(angle) {}

SpawnPoint::SpawnPoint() {}

/**
* MapObstacle.
*/

MapObstacle::MapObstacle() {}

MapObstacle::MapObstacle(const sf::Vector2f &pos,
                         const std::vector<sf::Vector2f> &localVertices,
                         const float damage) :
    pos(pos), localVertices(localVertices), decomposed(), damage(damage) {
  decompose();
}

void MapObstacle::decompose() {
  decomposed.clear();

  std::vector<sf::Vector2f> verts(localVertices);
  pp::Poly poly(verts);
  poly.SetOrientation(TPPL_CCW);
  std::list<pp::Poly> tmp;

  pp::Partition part;
  part.ConvexPartition_HM(&poly, &tmp);

  for (auto p : tmp) {
    decomposed.push_back(p.GetPoints());
  }
}

/**
 * MapItem.
 */

/**
 * Map.
 */

Map::Map(std::istream &stream) :
    dimensions(3200, 900),
    loadSuccess(true) {
  try {
    cereal::JSONInputArchive ar(stream);
    serialize(ar);
  } catch (cereal::Exception e) {
    appLog("Failed to parse map!", LogOrigin::Engine);
    loadSuccess = false;
    return;
  }

  for (auto &o : obstacles) {
    o.decompose();
  }
}

Map::Map() :
    dimensions(1600, 900),
    loadSuccess(true) {}

const sf::Vector2f &Map::getDimensions() const {
  return dimensions;
}

const std::vector<MapObstacle> &Map::getObstacles() const {
  return obstacles;
}

const std::vector<MapItem> &Map::getItems() const {
  return items;
}

const std::vector<SpawnPoint> &Map::getSpawnPoints() const {
  return spawnPoints;
}

const SpawnPoint Map::pickSpawnPoint(const Team team) const {
  if (spawnPoints.size() > 0) {
    return spawnPoints[0];
  } else {
    return SpawnPoint({200, 200}, 0); // default
  }
}

void Map::save(std::ostream &s) {
  cereal::JSONOutputArchive ar(s);
  serialize(ar);
}

optional<Map> Map::load(std::istream &stream) {
  Map map{stream};
  if (map.loadSuccess) return map;
  else return {};
}

}

