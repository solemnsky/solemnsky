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
 * A zone that planes can land on to rest / replenish health activate mechanics.
 */
#pragma once
#include "component.hpp"

namespace sky {

/**
 * Data describing a HomeBase.
 */
struct HomeBaseState {

};

/**
 * Delta type for HomeBase.
 */

struct HomeBaseDelta {
  optional<bool> exists;

  template<typename Archive>
  void serialize(Archive &ar) { }

};

class HomeBase: public Component<HomeBaseState, HomeBaseDelta> {
  friend class Sky;
 private:
  // Component impl.

 protected:
  void prePhysics() override final;
  void postPhysics(const TimeDiff delta) override final;

 public:
  HomeBase(const HomeBaseState &state, Physics &physics);

  // AutoNetworked impl.

};

}

