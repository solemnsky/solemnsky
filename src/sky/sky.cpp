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
  return true; // TODO: initializer invariants?
}

/**
 * SkyDelta.
 */

bool SkyDelta::verifyStructure() const {
  for (auto const &x : planes)
    if (!x.second.verifyStructure()) return false;
  return true;
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
    Subsystem(arena),
    map(arena.getMap()),
    physics(map, *this) {
  arena.forPlayers([&](Player &player) {
    const auto iter = initializer.planes.find(player.pid);
    if (iter != initializer.planes.end()) {
      registerPlayerWith(player, iter->second);
    } else {
      registerPlayer(player);
    }
  });
}

const Map &Sky::getMap() const {
  return map;
}

const Participation &Sky::getParticipation(const Player &player) const {
  return getPlayerData(player);
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
  appLog("Loading map " + inQuotes(arena.getMap()) + ".", LogOrigin::Engine);
  sky.emplace(arena);
}

SkyHandle::SkyHandle(Arena &arena, const SkyInitializer &initializer) :
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
      initializer.planes.emplace(
          pair.first, pair.second.captureInitializer());
    return initializer;
  } else return {};
}

SkyDelta SkyHandle::collectDelta() {
  if (sky) {
    SkyDelta delta;
    for (auto &pair : sky->participations)
      delta.planes.emplace(pair.first, pair.second->captureDelta());
    return delta;
  } else return {};
}

void SkyHandle::applyDelta(const SkyDelta &delta) {
  for (auto const &pair : delta.planes) {
    if (optional<Participation>
        *plane = findValue(sky->participations, pair.first)) {
      if (*plane) (*plane)->applyDelta(pair.second);
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
