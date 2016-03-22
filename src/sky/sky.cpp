#include "arena.h"
#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * SkyInitializer.
 */

SkyInitializer::SkyInitializer() { }

/**
 * SkyDelta.
 */

SkyDelta::SkyDelta() { }

bool SkyDelta::verifyStructure() const {
  for (auto const &x : state)
    if (!x.second.verifyStructure()) return false
  return true;
}

/**
 * Sky.
 */

Sky::Sky(Arena *arena, const MapName &mapName) :
    Subsystem(arena),
    mapName(mapName),
    map(mapName),
    physics(map) {
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

Plane &Sky::addPlane(const PID pid) {
  Plane &plane = planes.emplace(pid, Plane(this)).first->second;
  return plane;
}

void Sky::removePlane(const PID pid) {
  planes.erase(pid);
}

Plane *Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end()) return &planes.at(pid);
  else return nullptr;
}

void Sky::tick(float delta) {
  for (auto &elem : planes) {
    elem.second.beforePhysics();
  }
  physics.tick(delta);
  for (auto &elem : planes) {
    elem.second.afterPhysics(delta);
  }
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
    delta.state.emplace(pair.first, pair.second.captureDelta());
  return delta;
}

void Sky::applyDelta(const SkyDelta &delta) {
  for (auto const &pair : delta.state) {
    if (Plane *plane = getPlane(pair.first)) {
      plane->applyDelta(pair.second);
    }
  }
}

}
