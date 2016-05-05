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
         std::map<PID, optional<SkyPlayer>> &skyPlayers) :
    arena(arena),
    map(arena.getMap()),
    physics(map, *this),
    skyPlayers(skyPlayers) { }

/**
 * SkyHolder.
 */

void SkyHolder::registerPlayerWith(Player &player,
                                   const SkyPlayerInit &initializer) {
  if (!findValue(skyPlayers, player.pid)) {
    optional<SkyPlayer> *newPlayer;
    if (sky) {
      skyPlayers.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(player.pid),
          std::forward_as_tuple(sky->physics,
                                initializer));
    } else {
      skyPlayers.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(player.pid),
          std::forward_as_tuple());
    }
  }
  player.data.push_back(findValue(skyPlayers, player.pid));
}

void SkyHolder::registerPlayer(Player &player) {
  registerPlayerWith(player, {});
}

void SkyHolder::unregisterPlayer(Player &player) {
  const auto plane = skyPlayers.find(player.pid);
  if (plane != skyPlayers.end()) skyPlayers.erase(plane);
}

void SkyHolder::onTick(const float delta) {
  if (sky) sky->onTick(delta);

}

void SkyHolder::onJoin(Player &player) { }

void SkyHolder::onQuit(Player &player) { }

void SkyHolder::onMode(const ArenaMode newMode) {
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

void SkyHolder::onMapChange() {
  if (arena.getMode() == ArenaMode::Game) restart();
}

void SkyHolder::onAction(Player &player,
                         const Action action,
                         const bool state) {
  getPlane(player).doAction(action, state);
}

void SkyHolder::onSpawn(Player &player,
                        const PlaneTuning &tuning,
                        const sf::Vector2f &pos,
                        const float rot) {
  getPlane(player).spawn(tuning, pos, rot);
}

void SkyHolder::stop() {
  if (sky) {
    appLog("Stopping sky.");
    for (auto &pair : skyPlayers) {
      pair.second.reset();
    }
    sky.reset();
  }
}

void SkyHolder::start() {
  if (sky) stop();
  appLog("Loading map " + inQuotes(arena.getMap()) + ".", LogOrigin::Engine);
  sky.emplace(arena, skyPlayers);
  for (auto &pair : skyPlayers) {
    pair.second.emplace(sky->physics, SkyPlayerInit());
  }
}

SkyHolder::SkyHolder(Arena &arena, const SkyInitializer &initializer) :
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

SkyHolder::~SkyHolder() {
  stop(); // necessary to correctly free the box2d world
}

SkyInitializer SkyHolder::captureInitializer() {
  if (sky) {
    SkyInitializer initializer;
    for (const auto &pair : skyPlayers)
      initializer.planes.emplace(
          pair.first, pair.second->captureInitializer());
    return initializer;
  } else return {};
}

SkyDelta SkyHolder::collectDelta() {
  if (sky) {
    SkyDelta delta;
    for (auto &pair : skyPlayers)
      delta.planes.emplace(pair.first, pair.second->captureDelta());
    return delta;
  } else return {};
}

void SkyHolder::applyDelta(const SkyDelta &delta) {
  for (auto const &pair : delta.planes) {
    if (SkyPlayer *plane = planeFromPID(pair.first)) {
      plane->applyDelta(pair.second);
    }
  }
}

SkyPlayer &SkyHolder::getPlane(const Player &player) const {
  return getPlayerData<SkyPlayer>(player);
}

void SkyHolder::restart() {
  stop();
  start();
}

}
