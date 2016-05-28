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
#include "sky.h"
#include "util/printer.h"

namespace sky {

/**
 * SkyInit.
 */

SkyInit::SkyInit(const MapName &mapName) :
    mapName(mapName) { }

bool SkyInit::verifyStructure() const {
  return verifyMap(participations);
}

/**
 * SkyDelta.
 */

bool SkyDelta::verifyStructure() const {
  return verifyMap(participations);
}

/**
 * Sky.
 */

void Sky::registerPlayerWith(Player &player,
                             const ParticipationInit &initializer) {
  participations.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(player.pid),
      std::forward_as_tuple(physics, initializer));
  setPlayerData(player, participations.find(player.pid)->second);
}

void Sky::registerPlayer(Player &player) {
  registerPlayerWith(player, {});
}

void Sky::unregisterPlayer(Player &player) {
  participations.erase(participations.find(player.pid));
}

void Sky::onTick(const TimeDiff delta) {
  for (auto &p: participations) p.second.prePhysics();
  physics.tick(delta);
  for (auto &p: participations) p.second.postPhysics(delta);
}

void Sky::onAction(Player &player, const Action action, const bool state) {
  getPlayerData(player).doAction(action, state);
}

void Sky::onSpawn(Player &player,
                  const PlaneTuning &tuning,
                  const sf::Vector2f &pos,
                  const float rot) {
  getPlayerData(player).spawn(tuning, pos, rot);
}

void Sky::onBeginContact(const BodyTag &body1, const BodyTag &body2) {
  if (body1.type == BodyTag::Type::PlaneTag)
    body1.plane->onBeginContact(body2);
  if (body2.type == BodyTag::Type::PlaneTag)
    body2.plane->onBeginContact(body1);
}

void Sky::onEndContact(const BodyTag &body1, const BodyTag &body2) {
  if (body1.type == BodyTag::Type::PlaneTag)
    body1.plane->onEndContact(body2);
  if (body2.type == BodyTag::Type::PlaneTag)
    body2.plane->onEndContact(body1);
}

Sky::Sky(Arena &arena, const SkyInit &initializer) :
    Subsystem(arena),
    Networked(initializer),
    map(initializer.mapName),
    physics(map, *this) {
  arena.forPlayers([&](Player &player) {
    const auto iter = initializer.participations.find(player.pid);
    registerPlayerWith(
        player,
        (iter == initializer.participations.end()) ?
        ParticipationInit{} : iter->second);
  });

  appLog("Started game on " + map.name, LogOrigin::Engine);
}

void Sky::applyDelta(const SkyDelta &delta) {
  for (const auto &participation: delta.participations) {
    if (const Player *player = arena.getPlayer(participation.first)) {
      getPlayerData(*player).applyDelta(participation.second);
    }
  }
}

SkyInit Sky::captureInitializer() const {
  SkyInit initializer;
  initializer.mapName = map.name;
  for (const auto &participation : participations)
    initializer.participations.emplace(
        participation.first, participation.second.captureInitializer());
  return initializer;
}

SkyDelta Sky::collectDelta() {
  SkyDelta delta;
  for (auto &participation : participations) {
    delta.participations.emplace(
        participation.first, participation.second.collectDelta());
  }
  return delta;
}

const Map &Sky::getMap() const {
  return map;
}

Participation &Sky::getParticipation(const Player &player) const {
  return getPlayerData(player);
}


}
