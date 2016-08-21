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

namespace sky {

/**
 * EntityState.
 */
EntityState::EntityState(const optional<MovementLaws> movement,
                         const FillStyle &fill,
                         const Shape &shape,
                         const sf::Vector2f &pos,
                         const sf::Vector2f &vel) :
    movement(movement),
    fill(fill),
    shape(shape),
    physical(pos, vel, Angle(0), 0),
    lifetime(0) { }

/**
 * Entity.
 */

void Entity::prePhysics() {
  state.physical.writeToBody(physics, body);
}

void Entity::postPhysics(const TimeDiff delta) {
  state.physical.readFromBody(physics, body);
  state.lifetime += delta;
  if (const auto &movement = state.movement)
    movement->tick(delta, state.physical);
}

Entity::Entity(const EntityState &state, Physics &physics) :
    Component(state, physics),
    body(physics.createBody(state.shape, BodyTag::EntityTag(*this))) {
  state.physical.hardWriteToBody(physics, body);
  body->SetGravityScale(0);
}

EntityState Entity::captureInitializer() const {
  return state;
}

void Entity::applyDelta(const EntityDelta &delta, const TimeDiff delay) {
  state.physical = delta.physical;
}

optional<EntityDelta> Entity::collectDelta() {
  EntityDelta delta;
  delta.physical = state.physical;
  return delta;
}

const EntityState &Entity::getState() const {
  return state;
}

}
