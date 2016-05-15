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
 * A prop, a mutable-lifetime (projectile, etc) entity that a Plane can own.
 * Subordinate to a Plane.
 */
#pragma once
#include "physics.h"

namespace sky {

/**
 * Currently just a bullet.
 */
class Prop {
  friend class Participation;
 private:
  // State.
  Physics &physics;
  b2Body *const body;
  PhysicalState physical;
  float lifetime;

  // Sky API.
  void writeToBody();
  void readFromBody();
  void tick(const float delta);

 public:
  Prop() = delete;
  Prop(Physics &physics,
       const sf::Vector2f &pos,
       const sf::Vector2f &vel);
  ~Prop();

  // User API.
  const PhysicalState &getPhysical() const;
  float getLifetime() const;

};

}
