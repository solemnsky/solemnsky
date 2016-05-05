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

}

void Sky::onEndContact(const BodyTag &body1, const BodyTag &body2) {

}

Sky::Sky(const Arena &arena,
         std::map<PID, optional<SkyPlayer>> &skyPlayers) :
    arena(arena), skyPlayers(skyPlayers) { }

/**
 * SkyHolder.
 */

void SkyHolder::registerPlayerWith(Player &player,
                                   const SkyPlayerInit &initializer) {
  const auto plane = skyPlayers.find(player.pid);
  if (plane == skyPlayers.end()) {
    skyPlayers.emplace(std::piecewise_construct,
                       std::forward_as_tuple(player.pid),
                       std::forward_as_tuple(*this, player, initializer));
    player.data.push_back(&skyPlayers.at(player.pid));
  } else player.data.push_back(&plane->second);
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
  if (physics) {
    appLog("Stopping sky.");
    for (auto &pair : skyPlayers) {
      pair.second.reset();
    }
    map.reset();
    physics.reset();
  }
}

void SkyHolder::start() {
  if (physics) stop();
  appLog("Loading map " + inQuotes(arena.getMap()) + ".", LogOrigin::Engine);
  map.emplace(arena.getMap());
  physics.emplace(map.get(), *this);
}

void SkyHolder::onBeginContact(const BodyTag &body1, const BodyTag &body2) {
  if (body1.type == BodyTag::Type::PlaneTag)
    body1.planeVital->onBeginContact(body2);
  if (body2.type == BodyTag::Type::PlaneTag)
    body2.planeVital->onBeginContact(body1);
}

void SkyHolder::onEndContact(const BodyTag &body1, const BodyTag &body2) {
  if (body1.type == BodyTag::Type::PlaneTag)
    body1.planeVital->onEndContact(body2);
  if (body2.type == BodyTag::Type::PlaneTag)
    body2.planeVital->onEndContact(body1);
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

SkyPlayer *SkyHolder::planeFromPID(const PID pid) {
  auto plane = skyPlayers.find(pid);
  if (plane != skyPlayers.end()) return &plane->second;
  return nullptr;
}

SkyInitializer SkyHolder::captureInitializer() {
  SkyInitializer initializer;
  for (const auto &pair : skyPlayers)
    initializer.planes.emplace(pair.first, pair.second.captureInitializer());
  return initializer;
}

SkyDelta SkyHolder::collectDelta() {
  SkyDelta delta;
  for (auto &pair : skyPlayers)
    delta.planes.emplace(pair.first, pair.second.captureDelta());
  return delta;
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
