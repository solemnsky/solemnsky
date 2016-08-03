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
 * Any non-player entity in the game.
 */
#pragma once
#include "engine/multimedia.hpp"
#include "physics.hpp"
#include "movement.hpp"

namespace sky {

/**
 * All the data that defines an entity.
 */
struct EntityData {
  // Constants.
  const optional<Movement> movement;
  const FillStyle fill;

  // Variables.
  PhysicalState physical;

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(movement, fill, physical);
  }

};


/**
 * Delta for Entity.
 */
struct EntityDelta {
  EntityDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(physical);
  }

  PhysicalState physical;

};

/**
 * Something a player can create.
 */
class Entity: public Networked<EntityInit, EntityDelta> {
  friend class Participation;
 private:
  // Settings.
  optional<Movement> movement; // If this doesn't exist, it's fixed.

  // State.
  Physics &physics;
  b2Body *const body;
  PhysicalState physical;
  float lifetime;
  bool destroyable;

  // Delta collection state, for Participation.
  bool newlyAlive;

  // Sky API.
  void writeToBody();
  void readFromBody();
  void tick(const TimeDiff delta);

 public:
  Entity() = delete;
  Entity(class Player &player,
         Physics &physics,
         const EntityInit &initializer);

  // Associated player.
  class Player &player;

  // Networked API.
  EntityInit captureInitializer() const override final;
  void applyDelta(const EntityDelta &delta) override final;
  EntityDelta collectDelta();

  // User API.
  const PhysicalState &getPhysical() const;
  float getLifetime() const;
  void destroy();

};

}
