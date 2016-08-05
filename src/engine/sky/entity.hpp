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
 * Customizable representation of any non-player entity in the game.
 */
#pragma once
#include "engine/multimedia.hpp"
#include "physics.hpp"
#include "movement.hpp"

namespace sky {

/**
 * All the settings and state that defines an entity.
 */
struct EntityState {
  EntityState(const optional<MovementLaws> movement,
              const FillStyle &fill, const sf::Vector2f &pos, const sf::Vector2f &vel);

  // Constants.
  const optional<MovementLaws> movement; // The body is fixed if this does not exist.
  const FillStyle fill;

  // Variables.
  PhysicalState physical;
  Time lifetime;

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(movement, fill, physical);
  }

};

/**
 * The network initialized for Entity is just EntityState.
 * This alias makes this decision more explicit.
 */
using EntityInit = EntityState;

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
 * Some non-player entity.
 */
class Entity: public Networked<EntityInit, EntityDelta> {
  friend class Sky;
 private:
  // Data.
  EntityState state;

  // Physics.
  Physics &physics;
  b2Body *const body;

  // Delta collection state, for Participation.
  bool newlyAlive;

  // Sky API.
  void prePhysics();
  void postPhysics(const TimeDiff delta);

 public:
  Entity() = delete;
  Entity(const EntityInit &initializer, Physics &physics);

  // Networked API.
  EntityInit captureInitializer() const override final;
  void applyDelta(const EntityDelta &delta) override final;
  EntityDelta collectDelta();

  // User API.
  const EntityState &getState() const;
  void destroy();

  // Destroyable flag, for simulation on server.
  bool destroyable;

};

}
