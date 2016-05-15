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
#include "sky.h"

namespace sky {

Prop::Prop(Physics &physics, const sf::Vector2f &pos,
           const sf::Vector2f &vel) :
    physics(physics),
    body(physics.createBody(physics.rectShape({10, 10}),
                            BodyTag::PropTag(*this))),
    physical(pos, vel, 0, 0),
    lifetime(0) {
  physical.hardWriteToBody(physics, body);
}

Prop::~Prop() {
  physics.deleteBody(body);
}

void Prop::writeToBody() {
  physical.writeToBody(physics, body);
}

void Prop::readFromBody() {
  physical.readFromBody(physics, body);
}

void Prop::tick(const float delta) {
  lifetime += delta;
}

const PhysicalState &Prop::getPhysical() const {
  return physical;
}

float Prop::getLifetime() const {
  return lifetime;
}

}
