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
 * Static obstacle geometry data for a Sky.
 */
#pragma once
#include <SFML/System.hpp>
#include <string>
#include <ostream>
#include <istream>
#include <cereal/cereal.hpp>
#include "util/types.hpp"

namespace sky {

/**
 * A position and angle in a map where a plane can spawn.
 */
struct SpawnPoint {
  SpawnPoint();
  SpawnPoint(const sf::Vector2f &pos,
             const Angle &angle);
  sf::Vector2f pos;
  Angle angle;
  Team team;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(cereal::make_nvp("pos", pos),
       cereal::make_nvp("angle", angle),
       cereal::make_nvp("team", team));
  }

};

/**
 * A shape that things can collide with.
 */
struct MapObstacle {
  MapObstacle();
  MapObstacle(const sf::Vector2f &pos,
              const std::vector<sf::Vector2f> &localVertices,
              const float damage);

  sf::Vector2f pos;
  std::vector<sf::Vector2f> localVertices;
  std::vector<std::vector<sf::Vector2f>> decomposed;
  float damage;

  void decompose();

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(cereal::make_nvp("pos", pos),
       cereal::make_nvp("localVertices", localVertices),
       cereal::make_nvp("damage", damage));
  }

};

/**
 * A map 'item' (currently unimplemented).
 */
struct MapItem {
  enum class Type {
    RedThingy, BlueThingy
  } type;

  sf::Vector2f pos;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(cereal::make_nvp("type", type),
       cereal::make_nvp("pos", pos));
  }

};

/**
 * A whole static environment for a Sky.
 */
struct Map {
 private:
  // State, all constant after loading.
  sf::Vector2f dimensions;
  std::vector<MapObstacle> obstacles;
  std::vector<SpawnPoint> spawnPoints;
  std::vector<MapItem> items;
  bool loadSuccess;

  Map(std::istream &s);

 public:
  Map(); // null map
  Map(const Map &map) = default;
  Map(Map &&map) = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(cereal::make_nvp("dimensions", dimensions),
       cereal::make_nvp("obstacles", obstacles),
       cereal::make_nvp("spawnPoints", spawnPoints));
  }

  // User API.
  const sf::Vector2f &getDimensions() const;
  const std::vector<MapObstacle> &getObstacles() const;
  const std::vector<MapItem> &getItems() const;
  const std::vector<SpawnPoint> &getSpawnPoints() const;
  const SpawnPoint pickSpawnPoint(const Team team) const;

  // Safe reading / saving from / to streams.
  void save(std::ostream &s);
  static optional<Map> load(std::istream &s);

};

}
