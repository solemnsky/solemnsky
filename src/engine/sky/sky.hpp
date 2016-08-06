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
#include "physics.hpp"
#include "participation.hpp"
#include "skysettings.hpp"
#include "engine/arena.hpp"
#include "skylistener.hpp"
#include "util/networkedmap.hpp"

namespace sky {

/**
 * Initializer for Sky.
 */
struct SkyInit: public VerifyStructure {
  SkyInit() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(settings, participations, entities, explosions);
  }

  bool verifyStructure() const;

  SkySettingsInit settings;
  std::map<PID, ParticipationInit> participations;

  NetMapInit<EntityInit> entities;
  NetMapInit<ExplosionInit> explosions;

};

/**
 * Delta for Sky. Broadcast by server, applied by clients.
 */
struct SkyDelta: public VerifyStructure {
  SkyDelta();

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(settings, participations, entities, explosions);
  }

  bool verifyStructure() const;

  optional<SkySettingsDelta> settings;

  std::map<PID, ParticipationDelta> participations;

  NetMapDelta<EntityInit, EntityDelta> entities;
  NetMapDelta<ExplosionInit, ExplosionDelta> explosions;

  // Transform to respect client authority.
  SkyDelta respectAuthority(const Player &player) const;

};

/**
 * Game world when a game is in session.
 */
class Sky: public PhysicsListener,
           public Subsystem<Participation>,
           public Networked<SkyInit, SkyDelta> {
  friend class SkyHandle;
  friend class Participation;
 private:
  // Resources loaded by SkyHandle.
  const Map &map;

  // State.
  SkySettings settings;
  Physics physics;

  NetMap<Entity, EntityInit, EntityDelta, Physics&> entities;
  NetMap<Explosion, ExplosionInit, ExplosionDelta> explosions;

  // GameHandler.
  SkyListener *listener;

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
  bool enableContact(const BodyTag &body1,
                     const BodyTag &body2) override final;

  // Syncing settings to potential state.
  void syncSettings();

 public:
  Sky(Arena &arena, Map &&map,
      const SkyInit &, SkyListener *) = delete; // Map can't be temp
  Sky(Arena &arena, const Map &map, const SkyInit &initializer, SkyListener *listener = nullptr);

  // Player participations in the sky.
  std::map<PID, Participation> participations;

  // Networked impl.
  void applyDelta(const SkyDelta &delta) override final;
  SkyInit captureInitializer() const override final;
  SkyDelta collectDelta();

  // User API: reading state.
  const Map &getMap() const;
  Participation &getParticipation(const Player &player) const;
  const SkySettings &getSettings() const;
  void changeSettings(const SkySettingsDelta &delta);

  NetMapData<Entity> getEntities();
  NetMapData<Explosion> getExplosions();

  // User API: mutating state, server-only.
  void spawnEntity(const EntityInit &init);
  void spawnExplosion(const ExplosionInit &init);

};

}
