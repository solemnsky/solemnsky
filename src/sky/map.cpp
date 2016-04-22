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
  dimensions = {500, 500};
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
