#include "arena.h"
#include "sky.h"
#include "util/methods.h"

namespace sky {

SkyInitializer::SkyInitializer(const MapName &mapName) :
    mapName(mapName) { }

bool SkyDelta::verifyStructure() const {
  for (auto const &x : planes)
    if (!x.second.verifyStructure()) return false;
  return true;
}

void Sky::tick(const float delta) {
  for (auto &elem : planes) {
    elem.second.beforePhysics();
  }
  physics.tick(delta);
  for (auto &elem : planes) {
    elem.second.afterPhysics(delta);
  }
}

void *Sky::attachData(Player &player) {
  return nullptr;
}

void Sky::join(Player &player) {
  Plane &plane = planes.emplace(player.pid, Plane(this)).first->second;
  player.data.push_back(&plane);
}

void Sky::quit(Player &player) {
  planes.erase(player.pid);
}

Sky::Sky(Arena *arena, const SkyInitializer &initializer) :
    Subsystem(arena),
    mapName(initializer.mapName),
    map(mapName),
    physics(map) {
  // construct planes
  for (const auto &pair : initializer.planes)
    planes.emplace(pair.first, Plane(this, pair.second));
}

Sky::~Sky() {
  planes.clear(); // destroy the planes before destroying the physics!
}

Plane *Sky::getPlane(const PID pid) {
  auto plane = planes.find(pid);
  if (plane != planes.end()) return &plane->second;
  return nullptr;
}

SkyInitializer Sky::captureInitializer() {
  SkyInitializer initializer;
  initializer.mapName = mapName;
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
    if (Plane *plane = getPlane(pair.first)) {
      plane->applyDelta(pair.second);
    }
  }
}

}
