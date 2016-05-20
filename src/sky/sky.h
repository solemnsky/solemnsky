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
 * Physical game state of an Arena.
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
struct SkyInitializer: public VerifyStructure {
  SkyInitializer() = default;
  SkyInitializer(const MapName &mapName);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(mapName, participations);
  }

  bool verifyStructure() const;

  MapName mapName;
  std::map<PID, ParticipationInit> participations;

};

/**
 * Delta for Sky.
 */
struct SkyDelta: public VerifyStructure {
  SkyDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(participations);
  }

  bool verifyStructure() const;

  std::map<PID, ParticipationDelta> participations;

};

/**
 * Game world when a game is in session.
 */
class Sky
    : public PhysicsListener, public Subsystem<Participation>,
      public Networked<SkyInitializer, SkyDelta> {
  friend class SkyHandle;
  friend class Participation;
 private:
  // State.
  Map map;
  Physics physics;
  std::map<PID, Participation> participations;

 protected:
  // Internal API.
  void onTick(const float delta);

  // Subsystem impl.
  void registerPlayerWith(Player &player,
                          const ParticipationInit &initializer);
  void registerPlayer(Player &player) override final;
  void unregisterPlayer(Player &player) override final;
  void onAction(Player &player,
                const Action action,
                const bool state) override final;
  virtual void onSpawn(Player &player,
                       const PlaneTuning &tuning,
                       const sf::Vector2f &pos,
                       const float rot) override final;


  // PhysicsListener impl.
  virtual void onBeginContact(const BodyTag &body1,
                              const BodyTag &body2) override final;
  virtual void onEndContact(const BodyTag &body1,
                            const BodyTag &body2) override final;

 public:
  Sky(Arena &&arena, std::map<PID, optional<Participation>> &) = delete;
  Sky(Arena &arena, const SkyInitializer &initializer);

  // Networked impl.
  void applyDelta(const SkyDelta &delta) override final;
  SkyInitializer captureInitializer() const override final;
  SkyDelta collectDelta();

  // User API.
  const Map &getMap() const;
  const Participation &getParticipation(const Player &player) const;

};

/**
 * Initializer for SkyHandle.
 */

struct SkyHandleInit {
  SkyHandleInit() = default;
  explicit SkyHandleInit(const SkyInitializer &initializer);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(initializer);
  }

  bool verifyStructure() const;

  optional<SkyInitializer> initializer;
};

/**
 * Delta for SkyHandle.
 */

struct SkyHandleDelta {
  SkyHandleDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(initializer, delta);
  }

  bool verifyStructure() const;

  optional<SkyInitializer> initializer;
  optional<SkyDelta> delta;
};

/**
 * Wraps an optional<Sky>, binding it to the arena when the game is in session.
 * Also wraps its Networked implementation.
 */
class SkyHandle
    : public Subsystem<Nothing>,
      public Networked<SkyHandleInit, SkyHandleDelta> {
 private:
  // State.
  optional<Sky> sky;
  bool skyIsNew;

 public:
  SkyHandle(class Arena &parent, const SkyHandleInit &initializer);

  // Networking.
  SkyHandleInit captureInitializer() const override final;
  SkyHandleDelta collectDelta();
  void applyDelta(const SkyHandleDelta &delta) override final;

  // User API.
  void start();
  void stop();

  const optional<Sky> &getSky() const;
  bool isActive() const;

};

}
