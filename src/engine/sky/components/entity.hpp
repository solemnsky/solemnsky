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
 * Customizable representation of some free-moving object in the game.
 */
#pragma once
#include "engine/multimedia.hpp"
#include "util/networked.hpp"
#include "engine/sky/physics/physics.hpp"
#include "engine/sky/physics/movement.hpp"
#include "component.hpp"

namespace sky {

/**
 * All the settings and state that define an entity.
 */
struct EntityState {
  EntityState() = default;
  EntityState(const optional<MovementLaws> movement,
              const FillStyle &fill,
              const Shape &shape,
              const sf::Vector2f &pos,
              const sf::Vector2f &vel);

  // Constants.
  optional<MovementLaws> movement; // The body is fixed if this does not exist.
  FillStyle fill;
  Shape shape;

  // Variables.
  PhysicalState physical;
  Time lifetime;

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(movement, fill, shape, physical, lifetime);
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
 * Some non-player entity.
 */
class Entity: public Component<EntityState, EntityDelta> {
  friend class Sky;
 private:
  b2Body *const body;

 protected:
  // Component impl.
  void prePhysics() override final;
  void postPhysics(const TimeDiff delta) override final;

 public:
  Entity() = delete;
  Entity(const EntityState &state, Physics &physics);

  // Networked impl.
  EntityState captureInitializer() const override final;
  void applyDelta(const EntityDelta &delta) override final;
  optional<EntityDelta> collectDelta() override final;

  // User API.
  const EntityState &getState() const;

};

}
