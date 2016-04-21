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

Map::Map(const MapName &name) :
    dimensions(3200, 900),
    obstacles() {
  // TODO: loading from file with cereal
  // we'll also probably want to make map loading threaded soon
}

}
