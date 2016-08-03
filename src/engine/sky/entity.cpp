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
 * EntityInit.
 */

EntityInit::EntityInit(const sf::Vector2f &pos, const sf::Vector2f &vel) :
    physical(pos, vel, Angle(0), 0) { }

/**
 * EntityDelta.
 */

/**
 * Entity.
 */

void Entity::writeToBody() {
  physical.writeToBody(physics, body);
}

void Entity::readFromBody() {
  physical.readFromBody(physics, body);
}

void Entity::tick(const TimeDiff delta) {
  lifetime += delta;
}

Entity::Entity(Player &player,
               Physics &physics,
               const EntityInit &initializer) :
    Networked(initializer),
    physics(physics),
    body(physics.createBody(physics.rectShape({10, 10}),
                            BodyTag::PropTag(*this, player))),
    physical(initializer.physical),
    lifetime(0),
    destroyable(false),
    newlyAlive(true),

    player(player) {
  physical.hardWriteToBody(physics, body);
  body->SetGravityScale(0);
}

EntityInit Entity::captureInitializer() const {
  EntityInit init;
  init.physical = physical;
  return init;
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
