#include "arena.h"
#include "sky.h"

namespace sky {

bool SkyDelta::verifyStructure() const {
  for (auto const &x : planes)
    if (!x.second.verifyStructure()) return false;
  return true;
}

void Sky::registerPlayer(Player &player) {
  const auto plane = planes.find(player.pid);
  if (plane == planes.end()) {
    planes.emplace(std::piecewise_construct,
                   std::forward_as_tuple(player.pid),
                   std::forward_as_tuple(*this, player));
    player.data.push_back(&planes.at(player.pid));
  } else player.data.push_back(&plane->second);
}

void Sky::unregisterPlayer(Player &player) {
  const auto plane = planes.find(player.pid);
  if (plane != planes.end()) planes.erase(plane);
}

void Sky::onTick(const float delta) {
  if (physics) {
    for (auto &elem : planes) {
      elem.second.beforePhysics();
    }
    physics->tick(delta);
    for (auto &elem : planes) {
      elem.second.afterPhysics(delta);
    }
  }
}

void Sky::onJoin(Player &player) { }

void Sky::onQuit(Player &player) { }

void Sky::onMode(const ArenaMode newMode) {
  if (physics) {
    if (newMode != ArenaMode::Game) {
      stop();
    }
  } else {
    if (newMode == ArenaMode::Game) {
      start();
    }
  }
}

void Sky::onAction(Player &player, const Action action, const bool state) {
  getPlane(player).doAction(action, state);
}

void Sky::onSpawn(Player &player,
                  const PlaneTuning &tuning,
                  const sf::Vector2f &pos,
                  const float rot) {
  getPlane(player).spawn(tuning, pos, rot);
}

void Sky::stop() {
  for (auto &pair : planes) {
    pair.second.reset();
  }
  physics.reset();
}

void Sky::start() {
  physics.emplace(arena.map);
}

Sky::Sky(Arena &arena, const SkyInitializer &initializer) :
    Subsystem(arena) {
  if (arena.mode == ArenaMode::Game) {
    physics.emplace(arena.map);

    for (auto &player : arena.players) {
      const auto iter = initializer.planes.find(player.first);
      if (iter != initializer.planes.end()) {
        // initialize with the PlaneInitializer
        planes.emplace(std::piecewise_construct,
                       std::forward_as_tuple(player.first),
                       std::forward_as_tuple(*this, player.second,
                                             iter->second));
      } else {
        // initialize default plane
        registerPlayer(player.second);
      }
    }
  } else {
    for (auto &player : arena.players) {
      registerPlayer(player.second);
    }
  }
}

Sky::~Sky() {
  stop(); // necessary to correctly free the box2d world
}

Plane *Sky::planeFromPID(const PID pid) {
  auto plane = planes.find(pid);
  if (plane != planes.end()) return &plane->second;
  return nullptr;
}

SkyInitializer Sky::captureInitializer() {
  SkyInitializer initializer;
  for (const auto &pair : planes)
    initializer.planes.emplace(pair.first, pair.second.captureInitializer());
  return initializer;
}

SkyDelta Sky::collectDelta() {
  SkyDelta delta;
  for (auto &pair : planes)
    delta.planes.emplace(pair.first, pair.second.captureDelta());
  return delta;
}

void Sky::applyDelta(const SkyDelta &delta) {
  for (auto const &pair : delta.planes) {
    if (Plane *plane = planeFromPID(pair.first)) {
      plane->applyDelta(pair.second);
    }
  }
}

Plane &Sky::getPlane(const Player &player) const {
  return getPlayerData<Plane>(player);
}

void Sky::restart() {
  stop();
  start();
}

}
