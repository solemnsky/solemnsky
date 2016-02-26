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
        tg::MemberRule<SkyInitializer, std::map<PID, Plane>>(
            tg::MapPack<PID, Plane>(pidPack, PlanePack()),
            &SkyInitializer::planes
        )
    ) { }
#undef member

/**
 * Snapshot.
 */

SkySnapshot::SkySnapshot() { }

SkyDeltaPack::SkyDeltaPack() :
    tg::ClassPack<SkySnapshot>() { }

/**
 * Sky.
 */

Sky::Sky(const Map &map) : map(map), physics(map) {
  CTOR_LOG("sky");
}

Sky::Sky(const SkyInitializer &initializer) :
    map(initializer.mapName),
    physics(map) { }

Sky::~Sky() {
  DTOR_LOG("sky");
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
  return (planes.emplace(pid, Plane(this, tuning, pos, rot))).first->second;
}

void Sky::removePlane(const PID pid) {
  planes.erase(pid);
  for (auto system : subsystems) system->removePlane(pid);
}

void Sky::fireLaser(const PID pid) {
  if (auto *plane = getPlane(pid)) {
    if (plane->state.requestDiscreteEnergy(0.3)) {
      appLog("PEW PEW");
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

}
