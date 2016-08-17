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
 * The Plane class, subcomponent of Participation.
 */
#pragma once

namespace sky {

/**
 * The plane element that can be associated with a Participation.
 * This is essentially a piece of Participations's implementation.
 */
class Plane {
  friend class Sky;
  friend class Participation;
 private:
  // Parameters.
  Physics &physics;
  const PlaneControls &controls;

  // State.
  const PlaneTuning tuning;
  PlaneState state;
  b2Body *const body;

  // Subroutines.
  void switchStall();
  void tickFlight(const TimeDiff delta);
  void tickWeapons(const TimeDiff delta);
  void writeToBody();
  void readFromBody();

  // Sky API.
  void prePhysics();
  void postPhysics(const TimeDiff delta);
  void onBeginContact(const BodyTag &body);
  void onEndContact(const BodyTag &body);

 public:
  Plane() = delete;
  Plane(class Player &, const PID player, Physics &, PlaneControls &&, const PlaneTuning &,
        const PlaneState &) = delete; // `controls` must not be a temp
  Plane(class Player &player,
        Physics &physics,
        const PlaneControls &controls,
        const PlaneTuning &tuning,
        const PlaneState &state);
  ~Plane();

  // Associated player.
  class Player &player;

  // User API.
  const PlaneTuning &getTuning() const;
  const PlaneState &getState() const;

  bool requestDiscreteEnergy(const float reqEnergy);
  float requestEnergy(const float reqEnergy);
  void resetPrimary();
  void damage(const float amount);

};

}
