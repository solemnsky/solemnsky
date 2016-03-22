#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * SkyInitializer.
 */

SkyInitializer::SkyInitializer() { }

/**
 * Snapshot.
 */

SkyDelta::SkyDelta() { }

/**
 * Sky.
 */

Sky::Sky(const MapName &mapName) :
    mapName(mapName),
    map(mapName),
    physics(map) {
  CTOR_LOG("Sky");
}

Sky::Sky(const SkyInitializer &initializer) :
    mapName(initializer.mapName),
    map(mapName),
    physics(map) {
  CTOR_LOG("Sky");
  // construct planes
  for (const auto &pair : initializer.planes)
    planes.emplace(pair.first, PlaneVital(this, pair.second));
}

Sky::~Sky() {
  DTOR_LOG("Sky");
  planes.clear(); // destroy the planes before destroying the physics!
}

void Sky::linkSystem(Subsystem *subsystem) {
  subsystems.push_back(subsystem);
}

PlaneVital &Sky::addPlane(const PID pid,
                          const PlaneTuning &tuning,
                          const sf::Vector2f pos,
                          const float rot) {
  PlaneVital &plane =
      planes.emplace(pid, PlaneVital(this, tuning, pos, rot)).first->second;
  for (auto system : subsystems) system->addPlane(pid, plane);
  restructure[pid] = &plane;
  return plane;
}

void Sky::removePlane(const PID pid) {
  for (auto system : subsystems) system->removePlane(pid);
  planes.erase(pid);
  restructure[pid] = nullptr;
}

Plane *Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end()) return &planes.at(pid);
  else return nullptr;
}

void Sky::tick(float delta) {
  for (auto &elem : planes) {
    PlaneVital &plane = elem.second;
    plane.writeToBody();
  }
  physics.tick(delta);
  for (auto &elem : planes) {
    PlaneVital &plane = elem.second;
    plane.readFromBody();
    plane.tick(delta);
  }
  for (auto system : subsystems) system->tick(delta);
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
  for (const auto &pair : planes)
    delta.state.emplace(pair.first, pair.second.state);
  for (const auto &pair : restructure) {
    if (!pair.second)
      delta.restructure.emplace(
          pair.first, optional<PlaneInitializer>());
    else
      delta.restructure.emplace(pair.first, pair.second->captureInitializer());
  }
  restructure.clear();
  return delta;
}

void Sky::applyDelta(const SkyDelta &delta) {
  for (const auto &pair : delta.restructure) {
    planes.erase(pair.first);
    if (pair.second) planes.emplace(pair.first, PlaneVital(this, *pair.second));
  }
  for (const auto &pair : delta.state) {
    if (PlaneVital *plane = getPlane(pair.first)) {
      plane->state = pair.second;
      plane->writeToBody();
    }
  }
}

}
