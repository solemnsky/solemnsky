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
#include <cmath>
#include <SFML/Graphics/Rect.hpp>
#include "sky.hpp"
#include "engine/arena.hpp"
#include "participation.hpp"
#include "util/printer.hpp"
#include "util/methods.hpp"
#include "engine/arena.hpp"

namespace sky {

/**
 * ParticipationInit.
 */

ParticipationInit::ParticipationInit(
    const PlaneControls &controls,
    const PlaneTuning &tuning,
    const PlaneState &state) :
    spawn(std::pair<PlaneTuning, PlaneState>(tuning, state)),
    controls(controls) { }

ParticipationInit::ParticipationInit(
    const PlaneControls &controls) :
    controls(controls) { }

/**
 * ParticipationDelta.
 */

bool ParticipationDelta::verifyStructure() const {
  return imply(bool(spawn), !bool(state));
}

ParticipationDelta ParticipationDelta::respectClientAuthority() const {
  ParticipationDelta delta{*this};
  if (state) {
    delta.serverState = state;
    delta.state.reset();
  }
  delta.controls.reset();
  return delta;
}

/**
 * Participation.
 */

void Participation::effectSpawn(const PlaneTuning &tuning,
                                const PlaneState &state) {
  plane.emplace(player, physics, controls, tuning, state);
  caller.doSpawn(player);
}

void Participation::effectKill() {
  plane.reset();
  caller.doKill(player);
}

void Participation::prePhysics() {
  if (role.server()) {
    if (controls.getState<Action::Suicide>()) suicide();
  }
  if (plane) plane->prePhysics();
}

void Participation::postPhysics(const float delta) {
  if (plane) plane->postPhysics(delta);
}

Participation::Participation(Player &player,
                             Physics &physics,
                             Role &role,
                             SubsystemCaller &caller,
                             const ParticipationInit &initializer) :
    AutoNetworked(initializer),
    physics(physics),
    role(role),
    caller(caller),
    controls(initializer.controls),
    newlyAlive(false),
    newlyDead(false),
    lastControls(initializer.controls),
    player(player) {
  if (initializer.spawn)
    effectSpawn(initializer.spawn->first, initializer.spawn->second);
}

void Participation::applyDelta(const ParticipationDelta &delta) {
  assert(role.client());

  // Apply plane spawn / state.
  if (delta.spawn) {
    effectSpawn(delta.spawn->first, delta.spawn->second);
  } else {
    if (plane) {
      if (delta.state) plane->state = delta.state.get();
      else if (delta.serverState)
        plane->state.applyServer(delta.serverState.get());
      else effectKill();
    }
  }

  // Modify controls.
  if (delta.controls) {
    controls = *delta.controls;
  }
}

ParticipationInit Participation::captureInitializer() const {
  assert(role.server());

  ParticipationInit init{controls};
  if (plane) {
    init.spawn.emplace(plane->tuning, plane->state);
  }
  return init;
}

optional<ParticipationDelta> Participation::collectDelta() {
  assert(role.server());

  ParticipationDelta delta;
  bool useful{false};

  if (plane) {
    if (newlyAlive) {
      delta.spawn.emplace(plane->tuning, plane->state);
      newlyAlive = false;
    } else {
      delta.state.emplace(plane->state);
    }
    useful = true;
  } else {
    if (newlyDead) {
      useful = true;
      newlyDead = false;
    }
  }

  if (controls != lastControls) {
    delta.controls = controls;
    lastControls = controls;
    useful = true;
  }

  if (useful) return delta;
  return {};
}

const PlaneControls &Participation::getControls() const {
  return controls;
}

bool Participation::isSpawned() const {
  return bool(plane);
}

void Participation::doAction(const Action action, bool actionState) {
  controls.doAction(action, actionState);
}

void Participation::suicide() {
  assert(role.server());
  if (plane) {
    newlyDead = true;
    effectKill();
  }
}

void Participation::spawn(const PlaneTuning &tuning,
                          const sf::Vector2f &pos,
                          const float rot) {
  assert(role.server());
  effectSpawn(tuning, PlaneState(tuning, pos, rot));
  newlyAlive = true;
}


void Participation::applyInput(const ParticipationInput &input) {
  assert(role.server());

  if (input.controls) {
    controls = input.controls.get();
  }

  if (plane) {
    if (input.planeState)
      plane->state.applyClient(input.planeState.get());
  }
}

optional<ParticipationInput> Participation::collectInput() {
  assert(role.client(player.pid));

  bool useful{false};
  ParticipationInput input;
  if (lastControls != controls) {
    useful = true;
    input.controls = controls;
    lastControls = controls;
  }
  if (plane) {
    useful = true;
    input.planeState.emplace(plane->getState());
  }
  if (useful) return input;
  else return {};
}

}

