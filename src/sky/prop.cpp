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
#include "prop.h"
#include "participation.h"

namespace sky {

/**
 * PropInit.
 */

PropInit::PropInit(const sf::Vector2f &pos, const sf::Vector2f &vel) :
    physical(pos, vel, Angle(0), 0) { }

/**
 * PropDelta.
 */

/**
 * Prop.
 */

void Prop::writeToBody() {
  physical.writeToBody(physics, body);
}

void Prop::readFromBody() {
  physical.readFromBody(physics, body);
}

void Prop::tick(const float delta) {
  lifetime += delta;
}

Prop::Prop(const PID associatedPlayer,
           Physics &physics,
           const PropInit &initializer) :
    Networked(initializer),
    physics(physics),
    body(physics.createBody(physics.rectShape({10, 10}),
                            BodyTag::PropTag(*this))),
    physical(initializer.physical),
    lifetime(0),
    destroyable(false),
    newlyAlive(true),
    associatedPlayer(associatedPlayer) {
  physical.hardWriteToBody(physics, body);
  body->SetGravityScale(0);
}

Prop::~Prop() {
  physics.deleteBody(body);
}

PropInit Prop::captureInitializer() const {
  PropInit init;
  init.physical = physical;
  return init;
}

void Prop::applyDelta(const PropDelta &delta) {
  physical = delta.physical;
}

PropDelta Prop::collectDelta() {
  PropDelta delta;
  delta.physical = physical;
  return delta;
}

const PhysicalState &Prop::getPhysical() const {
  return physical;
}

float Prop::getLifetime() const {
  return lifetime;
}

void Prop::destroy() {
  destroyable = true;
}

}
