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
 * Physical game state of an Arena. Attaches to a Map.
 */
#pragma once
#include <map>
#include <memory>
#include "physics.h"
#include "participation.h"
#include "arena.h"

namespace sky {

/**
 * Initializer for Sky.
 */
struct SkyInit: public VerifyStructure {
  SkyInit() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(participations);
  }

  bool verifyStructure() const;

  std::map<PID, ParticipationInit> participations;

};

/**
 * Delta for Sky. Broadcast by server, applied by clients.
 */
struct SkyDelta: public VerifyStructure {
  SkyDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(participations);
  }

  bool verifyStructure() const;

  std::map<PID, ParticipationDelta> participations;

  // Respecting client authority.
  SkyDelta respectAuthority(const Player &player) const;

};

/**
 * Game world when a game is in session.
 */
class Sky
    : public PhysicsListener, public Subsystem<Participation>,
      public Networked<SkyInit, SkyDelta> {
  friend class SkyHandle;
  friend class Participation;
 private:
  // State.
  const Map &map;
  Physics physics;
  std::map<PID, Participation> participations;

 protected:
  void registerPlayerWith(Player &player,
                          const ParticipationInit &initializer);
  // Subsystem impl.
  void registerPlayer(Player &player) override final;
  void unregisterPlayer(Player &player) override final;
  void onTick(const TimeDiff delta) override final;
  void onAction(Player &player,
                const Action action,
                const bool state) override final;
  void onSpawn(Player &player,
               const PlaneTuning &tuning,
               const sf::Vector2f &pos,
               const float rot) override final;

  // PhysicsListener impl.
  void onBeginContact(const BodyTag &body1,
                      const BodyTag &body2) override final;
  void onEndContact(const BodyTag &body1,
                    const BodyTag &body2) override final;

 public:
  Sky(Arena &arena, Map &&map,
      std::map<PID, optional<Participation>> &) = delete; // Map can't be temp
  Sky(Arena &arena, const Map &map, const SkyInit &initializer);

  // Networked impl.
  void applyDelta(const SkyDelta &delta) override final;
  SkyInit captureInitializer() const override final;
  SkyDelta collectDelta();

  // User API.
  const Map &getMap() const;
  Participation &getParticipation(const Player &player) const;

};

}
