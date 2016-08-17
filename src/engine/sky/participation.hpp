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
 * A player's Participation in the Sky.
 */
#pragma once
#include <Box2D/Box2D.h>
#include <forward_list>
#include "util/types.hpp"
#include "engine/sky/components/entity.hpp"
#include "engine/sky/physics/physics.hpp"
#include "engine/sky/components/explosion.hpp"
#include "engine/role.hpp"
#include "plane.hpp"

namespace sky {

/**
 * Initializer for Participation's Networked impl.
 */
struct ParticipationInit {
  ParticipationInit() = default;
  ParticipationInit(const PlaneControls &controls,
                    const PlaneTuning &tuning,
                    const PlaneState &state);
  ParticipationInit(const PlaneControls &controls);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(spawn, controls);
  }

  optional<std::pair<PlaneTuning, PlaneState>> spawn;
  PlaneControls controls;

};

/**
 * Delta for Participation's Networked impl.
 */
struct ParticipationDelta: public VerifyStructure {
  ParticipationDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(spawn, state, serverState, controls);
  }

  bool verifyStructure() const;

  optional<std::pair<PlaneTuning, PlaneState>> spawn;
  optional<PlaneState> state; // if client doesn't have authority
  optional<PlaneStateServer> serverState; // if client has authority
  optional<PlaneControls> controls; // client authority

  ParticipationDelta respectClientAuthority() const;

};

/**
 * Changes a client can apply to a server's Participation record for that
 * client.
 */
struct ParticipationInput {
  ParticipationInput() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(planeState, controls);
  }

  optional<PlaneStateClient> planeState;
  optional<PlaneControls> controls;

};

/**
 * A Player's participation in an active game.
 */
class Participation: public AutoNetworked<ParticipationInit, ParticipationDelta> {
  friend class Sky;
  friend class SkyHandle;
 private:
  // Parameters.
  Physics &physics;
  Role &role;
  class SubsystemCaller &caller;

  // Game state.
  PlaneControls controls;

  // Delta collection state.
  bool newlyAlive, newlyDead;
  PlaneControls lastControls;

  // Helpers.
  void effectSpawn(const PlaneTuning &tuning,
                   const PlaneState &state);
  void effectKill();

  // Sky API.
  void prePhysics();
  void postPhysics(const float delta);

 public:
  Participation() = delete;
  Participation(class Player &player,
                Physics &physics,
                Role &role,
                class SubsystemCaller &caller,
                const ParticipationInit &initializer);

  // State.
  class Player &player;
  optional<Plane> plane;

  // Networked impl (for Sky).
  void applyDelta(const ParticipationDelta &delta) override final;
  ParticipationInit captureInitializer() const override final;
  optional<ParticipationDelta> collectDelta() override final;

  // User API.
  const PlaneControls &getControls() const;
  bool isSpawned() const;

  // User API, client-side.
  void doAction(const Action action, bool actionState);

  // User API, server-side.
  void suicide();
  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f &pos, const float rot);

  // ParticipationInput.
  void applyInput(const ParticipationInput &input);
  optional<ParticipationInput> collectInput();

};

}
