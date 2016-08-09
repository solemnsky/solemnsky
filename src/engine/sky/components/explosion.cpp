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
#include "explosion.hpp"

namespace sky {

/**
 * ExplosionState.
 */

ExplosionState::ExplosionState() { }

/**
 * Explosion.
 */

void Explosion::prePhysics() {

}

void Explosion::postPhysics(const TimeDiff delta) {

}

Explosion::Explosion(const ExplosionState &state, Physics &physics) :
    Component(state, physics) { }

ExplosionState Explosion::captureInitializer() const {
  return ExplosionState();
}

void Explosion::applyDelta(const ExplosionDelta &delta) {

}

optional<ExplosionDelta> Explosion::collectDelta() {
  return Nothing();
}

}
