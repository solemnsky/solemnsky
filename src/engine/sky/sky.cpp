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
#include "sky.hpp"
#include "util/printer.hpp"

namespace sky {

/**
 * SkyInit.
 */

bool SkyInit::verifyStructure() const {
  return verifyMap(participations);
}

/**
 * SkyDelta.
 */

SkyDelta::SkyDelta() :
    settings(), participations() { }

bool SkyDelta::verifyStructure() const {
  return verifyMap(participations) and verifyOptionals(settings);
}

SkyDelta SkyDelta::respectAuthority(const Player &player) const {
  SkyDelta newDelta;
  newDelta.settings = settings;

  for (auto pDelta : participations) {
    if (pDelta.first == player.pid) {
      newDelta.participations.emplace(
          pDelta.first,
          pDelta.second.respectClientAuthority());
    } else {
      newDelta.participations.emplace(pDelta.first, pDelta.second);
    }
  }

  return newDelta;
}

/**
 * Sky.
 */

void Sky::registerPlayerWith(Player &player,
                             const ParticipationInit &initializer) {
  participations.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(player.pid),
      std::forward_as_tuple(player, physics, initializer));
  setPlayerData(player, participations.find(player.pid)->second);
}

void Sky::registerPlayer(Player &player) {
  registerPlayerWith(player, {});
}

void Sky::unregisterPlayer(Player &player) {
  participations.erase(participations.find(player.pid));
}

void Sky::onTick(const TimeDiff delta) {
  if (arena.serverResponsible()) {
    // Remove destroyable entities.
    // Only necessary if we're the server, client have no business doing this.
    auto entity = entities.begin();
    while (entity != entities.end()) {
      if (entity->second.destroyable) {
        const auto toErase = entity;
        ++entity;
        entities.erase(toErase);
      } else ++entity;
    }
  }

  // Synchronize state with box2d.
  for (auto &participation: participations) participation.second.prePhysics();
  for (auto &entity: entities) entity.second.prePhysics();
  for (auto &explosion: explosions) explosion.second.prePhysics();

  physics.tick(delta);

  // Tick everything.
  for (auto &participation: participations) participation.second.postPhysics(delta);
  for (auto &entity: entities) entity.second.postPhysics(delta);
  for (auto &explosion: explosions) explosion.second.postPhysics(delta);
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

//  for (auto s : arena.subsystems) {
//    if (s.second != this)
//      s.second->onBeginContact(body1, body2);
//  }
}

void Sky::onEndContact(const BodyTag &body1, const BodyTag &body2) {
  if (body1.type == BodyTag::Type::PlaneTag)
    body1.plane->onEndContact(body2);
  if (body2.type == BodyTag::Type::PlaneTag)
    body2.plane->onEndContact(body1);

//  for (auto s : arena.subsystems) {
//    if (s.second != this)
//      s.second->onEndContact(body1, body2);
//  }
}

bool Sky::enableContact(const BodyTag &body1, const BodyTag &body2) {
//  for (auto s : arena.subsystems) {
//    if (s.second != this)
//      if (!s.second->enableContact(body1, body2))
//        return false;
//  }
  return true;
}

void Sky::syncSettings() {
  physics.setGravity(settings.gravity);
}

Sky::Sky(Arena &arena, const Map &map, const SkyInit &initializer, SkyListener *listener) :
    Subsystem(arena),
    Networked(initializer),
    map(map),
    settings(initializer.settings),
    physics(map, *this),
    listener(listener) {
  arena.forPlayers([&](Player &player) {
    const auto iter = initializer.participations.find(player.pid);
    registerPlayerWith(
        player,
        (iter == initializer.participations.end()) ?
        ParticipationInit{} : iter->second);
  });

  for (const auto &entity : initializer.entities)
    entities.emplace(entity.first, entity.second);
  for (const auto &explosion: initializer.explosions)
    explosions.emplace(explosion.first, explosion.second);

  syncSettings();
  appLog("Instantiated Sky.", LogOrigin::Engine);
}

template<typename Data, typename Init, typename Delta>
void syncNetworkedMap(std::map<PID, Data> &map, std::map<PID, Init> &inits,
                      std::map<PID, Delta> &deltas, Physics &physics) {
  auto iter = map.begin();
  while (iter != map.end()) {
    if (deltas.find(iter->first) == deltas.end()) {
      const auto toErase = iter;
      ++iter;
      map.erase(toErase);
    } else ++iter;
  }
  for (const auto &init : inits) {
    map.emplace(std::piecewise_construct,
                std::forward_as_tuple(init.first),
                std::forward_as_tuple(physics, init.second));
  }
};

void Sky::applyDelta(const SkyDelta &delta) {
  // Participations.
  for (const auto &participation: delta.participations) {
    if (const Player *player = arena.getPlayer(participation.first)) {
      getPlayerData(*player).applyDelta(participation.second);
    }
  }

  // Settings.
  settings.applyDelta(delta.settings.get());

  // Entities.
  syncNetworkedMap(entities, delta.newEntities, delta.entities, physics);

  // Explosions.
  syncNetworkedMap(explosions, delta.newExplosions, delta.explosions, physics);

}

SkyInit Sky::captureInitializer() const {
  SkyInit initializer;
  for (const auto &participation : participations)
    initializer.participations.emplace(
        participation.first, participation.second.captureInitializer());

  for (const auto &entity : entities) {
    initializer.entities.emplace(entity.first, entity.second.captureInitializer());
  }

  for (const auto &explosion: explosions) {
    initializer.explosions.emplace(explosion.first, explosion.second.captureInitializer());
  }

  initializer.settings = settings.captureInitializer();
  return initializer;
}

SkyDelta Sky::collectDelta() {
  SkyDelta delta;
  for (auto &participation : participations) {
    delta.participations.emplace(
        participation.first, participation.second.collectDelta());
  }

  for (auto &entity: entities) {
    if (entity.second.newlyAlive) {
      delta.newEntities.emplace(entity.first, entity.second.captureInitializer());
    } else {
      delta.entities.emplace(entity.first, entity.second.collectDelta());
    }
  }

  for (auto &explosion: explosions) {
    delta.explosions.emplace(explosion.first, explosion.second.collectDelta());
  }

  delta.settings = settings.collectDelta();
  return delta;
}

const Map &Sky::getMap() const {
  return map;
}

Participation &Sky::getParticipation(const Player &player) const {
  return getPlayerData(player);
}

const SkySettings &Sky::getSettings() const {
  return settings;
}

void Sky::changeSettings(const SkySettingsDelta &delta) {
  settings.applyDelta(delta);
  syncSettings();
}

}
