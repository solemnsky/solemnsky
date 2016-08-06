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
 * ExplosionInit.
 */

ExplosionInit::ExplosionInit() { }

/**
 * ExplosionDelta.
 */

/**
 * Explosion.
 */

void Explosion::prePhysics() {

}

void Explosion::postPhysics(const TimeDiff delta) {

}

Explosion::Explosion(const ExplosionInit &init) :
    Networked(init) { }

ExplosionInit Explosion::captureInitializer() const {
  return sky::ExplosionInit();
}

void Explosion::applyDelta(const ExplosionDelta &delta) {

}

ExplosionDelta Explosion::collectDelta() {
  return sky::ExplosionDelta();
}

}
