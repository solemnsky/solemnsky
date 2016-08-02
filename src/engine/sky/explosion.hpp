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
 * An explosion in a Sky. Its effects are only simulated on the client of a participating plane.
 */
#pragma once
#include "physics.hpp"

namespace sky {

struct ExplosionInit {
  ExplosionInit();

  template<typename Archive>
  void serialize(Archive &ar) {
    ar();
  }
};

struct ExplosionDelta {
  template<typename Archive>
  void serialize(Archive &ar) {
    ar();
  }
};

struct Explosion: public Networked<ExplosionInit, ExplosionDelta> {
 public:
  Explosion(const ExplosionInit &init);

  // Networked impl.
  ExplosionInit captureInitializer() const override final;
  void applyDelta(const ExplosionDelta &delta) override final;
  ExplosionDelta collectDelta();

};

}
