#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * SkyInitializer.
 */

SkyInitializer::SkyInitializer() { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<SkyInitializer, TYPE>(RULE, &SkyInitializer::PTR)
SkyInitializerPack::SkyInitializerPack() :
    tg::ClassPack<SkyInitializer>(
        member(MapName, mapName, tg::stringPack),
        tg::MemberRule<SkyInitializer, std::map<PID, PlaneInitializer>>(
            tg::MapPack<PID, PlaneInitializer>(
                pidPack, PlaneInitializerPack()),
            &SkyInitializer::planes
        )
    ) { }
#undef member

/**
 * Snapshot.
 */

SkyDelta::SkyDelta() { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<SkyDelta, TYPE>(RULE, &SkyDelta::PTR)
SkyDeltaPack::SkyDeltaPack() :
    tg::ClassPack<SkyDelta>(
        // I'm glad I'm not implementing this without abstractions
        tg::MemberRule<SkyDelta, std::map<PID, optional<PlaneInitializer>>>(
            tg::MapPack<PID, optional<PlaneInitializer>>(
                pidPack,
                tg::OptionalPack<PlaneInitializer>(PlaneInitializerPack())),
            &SkyDelta::restructure
        ),
        tg::MemberRule<SkyDelta, std::map<PID, PlaneState>>(
            tg::MapPack<PID, PlaneState>(pidPack, PlaneStatePack()),
            &SkyDelta::state
        )
    ) { }
#undef member

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
    planes.emplace(pair.first, Plane(this, pair.second));
}

Sky::~Sky() {
  DTOR_LOG("Sky");
  planes.clear(); // destroy the planes before destroying the physics!
}

void Sky::linkSystem(Subsystem *subsystem) {
  subsystems.push_back(subsystem);
}

Plane *Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end()) return &planes.at(pid);
  else return nullptr;
}

Plane &Sky::addPlane(const PID pid,
                     const PlaneTuning &tuning,
                     const sf::Vector2f pos,
                     const float rot) {
  Plane &plane =
      planes.emplace(pid, Plane(this, tuning, pos, rot)).first->second;
  for (auto system : subsystems) system->addPlane(pid, plane);
  restructure[pid] = &plane;
  return plane;
}

void Sky::removePlane(const PID pid) {
  for (auto system : subsystems) system->removePlane(pid);
  planes.erase(pid);
  restructure[pid] = nullptr;
}

void Sky::fireLaser(const PID pid) {
  if (auto *plane = getPlane(pid)) {
    if (plane->state.requestDiscreteEnergy(0.3)) {
      appLog("PEW PEW wait for somebody to implement this PEW PEW PEW");
    }
  }
}

void Sky::tick(float delta) {
  for (auto &elem : planes) {
    Plane &plane = elem.second;
    plane.writeToBody();
  }
  physics.tick(delta);
  for (auto &elem : planes) {
    Plane &plane = elem.second;
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
    if (pair.second) planes.emplace(pair.first, Plane(this, *pair.second));
  }
  for (const auto &pair : delta.state) {
    if (Plane *plane = getPlane(pair.first)) {
      plane->state = pair.second;
      plane->writeToBody();
    }
  }
}

}
