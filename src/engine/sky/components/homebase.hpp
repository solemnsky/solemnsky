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
#include "engine/types.hpp"

namespace sky {

/*
 * Data describing a HomeBase.
 */
struct HomeBaseState {
  sf::Vector2f dimensions;
  sf::Vector2f pos;
  Angle rot;
  float damage;
  SwitchSet<Team> friendly;

  HomeBaseState() = default; // For serialization.
  HomeBaseState(const sf::Vector2f &dimensions,
                const sf::Vector2f &pos,
                const Angle rot,
                const float damage,
                const SwitchSet<Team> friendly);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(dimensions, pos, rot, damage, friendly);
  }

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
 protected:
  // Component impl.
  void prePhysics() override final;
  void postPhysics(const TimeDiff delta) override final;

 public:
  HomeBase(const HomeBaseState &state, Physics &physics);

  // AutoNetworked impl.
  void applyDelta(const HomeBaseDelta &delta) override final;
  HomeBaseState captureInitializer() const override final;


  virtual optional<HomeBaseDelta> collectDelta() override;
};

}

