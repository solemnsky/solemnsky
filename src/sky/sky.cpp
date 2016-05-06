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
  for (auto &p: skyPlayers) p.second->prePhysics();
  physics.tick(delta);
  for (auto &p: skyPlayers) p.second->postPhysics(delta);
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

Sky::Sky(const Arena &arena,
         std::map<PID, optional<Participation>> &skyPlayers) :
    arena(arena),
    map(arena.getMap()),
    physics(map, *this),
    skyPlayers(skyPlayers) { }

const Map &Sky::getMap() const {
  return map;
}

/**
 * SkyManager.
 */

void SkyManager::registerPlayerWith(Player &player,
                                    const ParticipationInit &initializer) {
  participations[player.pid].reset();
  if (sky) participations.at(player.pid).emplace(sky->physics, initializer);
  player.data.push_back(findValue(participations, player.pid));
}

optional<Participation> &SkyManager::accessParticipation(
    const Player &player) {
  return getPlayerData<optional<Participation>>(player);
}

void SkyManager::registerPlayer(Player &player) {
  registerPlayerWith(player, {});
}

void SkyManager::unregisterPlayer(Player &player) {
  const auto plane = participations.find(player.pid);
  if (plane != participations.end()) participations.erase(plane);
}

void SkyManager::onTick(const float delta) {
  if (sky) sky->onTick(delta);

}

void SkyManager::onJoin(Player &player) { }

void SkyManager::onQuit(Player &player) { }

void SkyManager::onMode(const ArenaMode newMode) {
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

void SkyManager::onMapChange() {
  if (arena.getMode() == ArenaMode::Game) restart();
}

void SkyManager::onAction(Player &player,
                          const Action action,
                          const bool state) {
  if (auto &p = accessParticipation(player))
    p->doAction(action, state);
}

void SkyManager::onSpawn(Player &player,
                         const PlaneTuning &tuning,
                         const sf::Vector2f &pos,
                         const float rot) {
  if (auto &p = accessParticipation(player))
    p->spawn(tuning, pos, rot);
}

void SkyManager::stop() {
  if (sky) {
    appLog("Stopping sky.");
    for (auto &pair : participations) {
      pair.second.reset();
    }
    sky.reset();
  }
}

void SkyManager::start() {
  if (sky) stop();
  appLog("Loading map " + inQuotes(arena.getMap()) + ".", LogOrigin::Engine);
  sky.emplace(arena, participations);
  for (auto &pair : participations) {
    pair.second.emplace(sky->physics, ParticipationInit());
  }
}

SkyManager::SkyManager(Arena &arena, const SkyInitializer &initializer) :
    Subsystem(arena) {

  if (arena.getMode() == ArenaMode::Game) {
    start();
    arena.forPlayers([&](Player &player) {
      const auto iter = initializer.planes.find(player.pid);
      if (iter != initializer.planes.end()) {
        registerPlayerWith(player, iter->second);
      } else {
        registerPlayer(player);
      }
    });
  } else {
    arena.forPlayers([&](Player &player) {
      registerPlayer(player);
    });
  }
}

SkyManager::~SkyManager() {
  stop(); // necessary to correctly free the box2d world
}

SkyInitializer SkyManager::captureInitializer() {
  if (sky) {
    SkyInitializer initializer;
    for (const auto &pair : participations)
      initializer.planes.emplace(
          pair.first, pair.second->captureInitializer());
    return initializer;
  } else return {};
}

SkyDelta SkyManager::collectDelta() {
  if (sky) {
    SkyDelta delta;
    for (auto &pair : participations)
      delta.planes.emplace(pair.first, pair.second->captureDelta());
    return delta;
  } else return {};
}

void SkyManager::applyDelta(const SkyDelta &delta) {
  for (auto const &pair : delta.planes) {
    if (optional<Participation>
        *plane = findValue(participations, pair.first)) {
      if (*plane) (*plane)->applyDelta(pair.second);
    }
  }
}

void SkyManager::restart() {
  stop();
  start();
}

const optional<Sky> &SkyManager::getSky() const {
  return sky;
}

bool SkyManager::isActive() const {
  return bool(sky);
}

const optional<Participation> &SkyManager::getParticipation(
    const Player &player) const {
  return getPlayerData<optional<Participation>>(player);
}

}
