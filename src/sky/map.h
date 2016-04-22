/**
 * The static environment in which a Sky plays out.
 * TODO: what if things move?
 */
#pragma once
#include <SFML/System.hpp>
#include <string>

namespace sky {

using MapName = std::string;

// a convex shape that planes collide with, ouch
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

struct Map {
 private:
  // Hard-coded maps for now.
  void loadTest1();
  void loadTest2();

 public:
  Map(const MapName &name);

  sf::Vector2f dimensions;
  std::vector<MapObstacle> obstacles;
  std::vector<MapItem> items;
};

}
