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
 * The laws of movement governing entities.
 */
#pragma once
#include "util/methods.hpp"
#include "engine/sky/physics/physics.hpp"

namespace sky {

struct MovementLaws {
  enum class Type {
    Free, // Free-fall, with potential damping at terminal velocity.
    Linear, // Move in a line, potentially bouncing off surfaces.
    Hover // Hover, like an explodet mine in altitude.
  } type;

  // Freefall.
  optional<std::pair<float, Clamped>> terminalDamping; // Damp velocity over a terminal point.

  // Linear.
  float velocity;
  bool enableBounce;

  // Hover.
  Clamped drag;

  // Applying to physics.
  b2Body *createBody(Physics &physics, const b2Shape &shape, const BodyTag &tag) const;
  void tick(const TimeDiff delta, PhysicalState &state) const;

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    switch (type) {
      case Type::Free: {
        ar(terminalDamping);
        break;
      }
      case Type::Linear: {
        ar(velocity, enableBounce);
      }
      case Type::Hover: {
        ar(drag);
      }
    }
  }

};

}
