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
#include "arena.h"
#include "sky.h"
#include "util/printer.h"

namespace sky {

/**
 * SkyInitializer.
 */

bool SkyInitializer::verifyStructure() const {
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

void Sky::onTick(const float delta) {
  for (auto &p: participations) p.second.prePhysics();
  physics.tick(delta);
  for (auto &p: participations) p.second.postPhysics(delta);
}

void Sky::registerPlayerWith(Player &player,
                             const ParticipationInit &initializer) {
  // participations.emplace(std::forward_as_tuple);
  // if (sky) participations.at(player.pid).emplace(sky->physics, initializer);
  // setPlayerData(player, *findValue(participations, player.pid));
}

void Sky::registerPlayer(Player &player) {
  registerPlayerWith(player, {});
}

void Sky::unregisterPlayer(Player &player) {
  const auto plane = participations.find(player.pid);
  if (plane != participations.end()) participations.erase(plane);
}

void Sky::onJoin(Player &player) {

}

void Sky::onQuit(Player &player) {

}

void Sky::onMode(const ArenaMode newMode) {

}

void Sky::onMapChange() {

}

void Sky::onAction(Player &player, const Action action, const bool state) {

}

void Sky::onSpawn(Player &player,
                  const PlaneTuning &tuning,
                  const sf::Vector2f &pos,
                  const float rot) {
  SubsystemListener::onSpawn(player, tuning, pos, rot);
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

Sky::Sky(Arena &arena, const SkyInitializer &initializer) :
    Networked(initializer),
    Subsystem(arena),
    map(initializer.mapName),
    physics(map, *this) {
  arena.forPlayers([&](Player &player) {
    const auto iter = initializer.participations.find(player.pid);
    if (iter != initializer.participations.end()) {
      registerPlayerWith(player, iter->second);
    } else {
      registerPlayer(player);
    }
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

SkyInitializer Sky::captureInitializer() const {
  return sky::SkyInitializer();
}

const Map &Sky::getMap() const {
  return map;
}

const Participation &Sky::getParticipation(const Player &player) const {
  return getPlayerData(player);
}

/**
 * SkyHandleInitializer.
 */

bool SkyHandleInitializer::verifyStructure() const {
  return verifyValue(initializer);
}

/**
 * SkyHandleDelta.
 */

bool SkyHandleDelta::verifyStructure() const {
  return verifyValue(delta);
}

/**
 * SkyHandle.
 */

void SkyHandle::onMode(const ArenaMode newMode) {
  if (sky) {
    if (newMode != ArenaMode::Game) {
      stop();
    }
  } else {
    if (newMode == ArenaMode::Game) {
      start();
    }
  }
}

void SkyHandle::onMapChange() {
  if (arena.getMode() == ArenaMode::Game) restart();
}

void SkyHandle::stop() {
  if (sky) sky.reset();
}

void SkyHandle::start() {
  if (sky) stop();
  sky.emplace(arena);
}

SkyHandle::SkyHandle(Arena &arena, const SkyHandleInitializer &initializer) :
    Networked(initializer),
    Subsystem(arena) {
  if (arena.getMode() == ArenaMode::Game) {
    start();
  } else {
    arena.forPlayers([&](Player &player) {
      registerPlayer(player);
    });
  }
}

SkyHandle::~SkyHandle() {
  stop(); // necessary to correctly free the box2d world
}

SkyInitializer SkyHandle::captureInitializer() {
  if (sky) {
    SkyInitializer initializer;
    for (const auto &pair : sky->participations)
      initializer.participations.emplace(
          pair.first, pair.second.captureInitializer());
    return initializer;
  } else return {};
}

SkyDelta SkyHandle::collectDelta() {
  SkyDelta delta;
  delta.isActive = isActive();

  if (sky) {
    for (auto &pair : sky->participations)
      delta.participations.emplace(pair.first, pair.second.captureDelta());
  }
  return delta;
}

void SkyHandle::applyDelta(const SkyDelta &delta) {
  if (sky) {
    for (const auto &participation : delta.participations) {
      if (const auto *player = arena.getPlayer(participation.first)) {
        sky->getPlayerData(*player).applyDelta(participation.second);
      }
    }
  }
}

void SkyHandle::restart() {
  stop();
  start();
}

const optional<Sky> &SkyHandle::getSky() const {
  return sky;
}

bool SkyHandle::isActive() const {
  return bool(sky);
}

const optional<Participation> &SkyHandle::getParticipation(
    const Player &player) const {
  return getPlayerData(player);
}

}
