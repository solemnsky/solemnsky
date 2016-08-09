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
 * Orthogonal representation of a component inside a Sky. Home bases, bullets, turrets, you name it.
 */
#pragma once
#include "util/types.hpp"
#include "util/networked.hpp"
#include "engine/sky/physics.hpp"

namespace sky {

template<typename Data, typename Delta>
class Component: public AutoNetworked<Data, Delta> {
  friend class Sky;
 protected:
  Data data;
  Physics &physics;

  // Callbacks from Sky.
  virtual void prePhysics() = 0;
  virtual void postPhysics(const TimeDiff delta) = 0;

 public:
  Component(const Data &data, Physics &physics) :
      AutoNetworked(data), data(data), physics(physics) { }

};

}
