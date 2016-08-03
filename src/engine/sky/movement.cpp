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
#include "movement.hpp"
#include "util/methods.hpp"

namespace sky {


b2Body *Movement::createBody(Physics &physics, const b2Shape &shape, const BodyTag &tag) {
  auto *body = physics.createBody(shape, tag, false);

  // Set additional physics options.
  switch (type) {
    case Type::Free: {
      break;
    }
    case Type::Linear: {
      body->SetGravityScale(0);
      if (enableBounce) {
        // uh, how do I set elasticity?
      }
      break;
    }
    case Type::Hover: {
      body->SetGravityScale(0);
    }
    default:
      throw enum_error();
  }

  return body;
}

void Movement::tick(const TimeDiff delta, Physics &physics, b2Body &body) {
  switch (type) {
    case Type::Free: {
      if (terminalDamping) {
        float maxVel = terminalDamping->first,
            dampingFactor = terminalDamping->second;
        if (VecMath::length(state.vel) > maxVel) {
          physics.approachVel()
        }

      }
    }
    default:
      break;
  }

}

}
