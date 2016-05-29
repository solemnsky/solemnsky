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
#include "util/printer.h"
#include <fstream>
#include <list>
#include <polypartition.h>

namespace sky {

/**
 * SpawnPoint.
 */

sky::SpawnPoint::SpawnPoint(const sf::Vector2f &pos, const Angle &angle):
  pos(pos), angle(angle){}

  SpawnPoint::SpawnPoint() { }

  /**
 * MapObstacle.
 */

MapObstacle::MapObstacle() { }

MapObstacle::MapObstacle(const sf::Vector2f &pos,
                         const std::vector<sf::Vector2f> &localVertices,
                         const float damage) :
    pos(pos), localVertices(localVertices), decomposed(), damage(damage) { }

void MapObstacle::decompose(){
  decomposed.clear();

  std::vector<sf::Vector2f> verts(localVertices);
  pp::Poly poly(verts);
  poly.SetOrientation(TPPL_CCW);
  std::list<pp::Poly> tmp;

  pp::Partition part;
  part.ConvexPartition_HM(&poly, &tmp);

  for(auto p : tmp){
    decomposed.push_back(p.GetPoints());
  }
}

/**
 * MapItem.
 */

/**
 * Map.
 */

Map::Map(const MapName &name) :
    dimensions(3200, 900),
    name(name) {
  auto file = std::ifstream(rootPath() + "maps/" + name + ".json");
  if(file.good()) {
    try {
      cereal::JSONInputArchive archive(file);
      archive(cereal::make_nvp("dimensions", dimensions),
              cereal::make_nvp("obstacles", obstacles),
              cereal::make_nvp("spawnPoints", spawnPoints));
    } catch(cereal::RapidJSONException e){
      appErrorRuntime("Failed to parse map '"+name+"', "+e.what());
    }
  } else {
    appErrorRuntime("Map '"+name+"' was not found.");
  }
  for(auto &o : obstacles){
    o.decompose();
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

const std::vector<SpawnPoint> &Map::getSpawnPoints() const {
  return spawnPoints;
}

void Map::save(std::ostream& s) {
  cereal::JSONOutputArchive archive(s);
  archive( cereal::make_nvp("dimensions",dimensions),
           cereal::make_nvp("obstacles",obstacles),
           cereal::make_nvp("spawnPoints",spawnPoints) );
}

}

