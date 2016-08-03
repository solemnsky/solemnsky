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
#include "entity.hpp"
#include "engine/player.hpp"

namespace sky {

/**
 * EntityState == EntityInit.
 */
EntityState::EntityState(const optional<MovementLaws> movement,
                         const FillStyle &fill,
                         const sf::Vector2f &pos,
                         const sf::Vector2f &vel) :
    movement(movement),
    fill(fill),
    physical(pos, vel, Angle(0), 0),
    lifetime(0) { }

/**
 * EntityDelta.
 */

/**
 * Entity.
 */

void Entity::writeToBody() {
  data..physical.writeToBody(physics, body);
}

void Entity::readFromBody() {
  data.physical.readFromBody(physics, body);
}

void Entity::tick(const TimeDiff delta) {
  data.lifetime += delta;
}

Entity::Entity(Player &player,
               Physics &physics,
               const EntityInit &initializer) :
    Networked(initializer),
    physics(physics),
    body(physics.createBody(physics.rectShape({10, 10}),
                            BodyTag::EntityTag(*this, player))),
    physical(initializer.physical),
    lifetime(0),
    newlyAlive(true),

    player(player), destroyable(false) {
  physical.hardWriteToBody(physics, body);
  body->SetGravityScale(0);
}

EntityInit Entity::captureInitializer() const {
  return data;
}

void Entity::applyDelta(const EntityDelta &delta) {
  physical = delta.physical;
}

EntityDelta Entity::collectDelta() {
  EntityDelta delta;
  delta.physical = physical;
  return delta;
}

const PhysicalState &Entity::getPhysical() const {
  return physical;
}

float Entity::getLifetime() const {
  return lifetime;
}

void Entity::destroy() {
  destroyable = true;
}

}
