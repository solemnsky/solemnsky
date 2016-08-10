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
 * An explosion in a Sky. Its effects are only simulated on the client of an affected plane.
 */
#pragma once
#include "engine/sky/physics/physics.hpp"
#include "component.hpp"

namespace sky {

/**
 * Explosion state data.
 */
struct ExplosionState {
  ExplosionState();

  template<typename Archive>
  void serialize(Archive &ar) { }
};

/**
 * Explosion delta type.
 */
using ExplosionDelta = Nothing;

struct Explosion: public Component<ExplosionState, ExplosionDelta> {
  friend class Sky;
 private:
  // Sky API.
  void prePhysics() override final;
  void postPhysics(const TimeDiff delta) override final;

 public:
  Explosion(const ExplosionState &init, Physics &physics);

  // Networked impl.
  ExplosionState captureInitializer() const override final;
  void applyDelta(const ExplosionDelta &delta) override final;
  optional<ExplosionDelta> collectDelta() override final;

};

}
