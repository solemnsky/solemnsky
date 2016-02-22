#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * SkyInitializer.
 */

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<SkyInitializer, TYPE>(RULE, &SkyInitializer::PTR)
const tg::Pack<SkyInitializer> skyInitializerPack =
    tg::ClassPack<SkyInitializer>(
        member(MapName, mapName, tg::stringPack),
        tg::MemberRule<SkyInitializer, std::map<PID, Plane>>(
            tg::MapPack<PID, Plane>(pidPack, planePack),
            &SkyInitializer::planes
        )
    );
#undef member

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

Plane &Sky::joinPlane(const PID pid) {
  planes.emplace(pid, std::move(PlaneHandle(this)));

  PlaneHandle &plane = planes.at(pid);
  for (auto system : subsystems) system->joinPlane(pid, plane);
  return plane.state;
}

Plane *Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end()) return &planes.at(pid).state;
  else return nullptr;
}

PlaneHandle *Sky::getPlaneHandle(const PID pid) {
  if (planes.find(pid) != planes.end()) return &planes.at(pid);
  else return nullptr;
}

void Sky::quitPlane(const PID pid) {
  planes.erase(pid);
  for (auto system : subsystems) system->quitPlane(pid);
}

void Sky::spawnPlane(const PID pid, const sf::Vector2f pos, const float rot,
                     const PlaneTuning &tuning) {
  if (auto *plane = getPlaneHandle(pid)) {
    plane->spawn(tuning, pos, rot);
    for (auto system : subsystems) system->spawnPlane(pid, *plane);
  }
}

void Sky::killPlane(const PID pid) {
  if (auto *plane = getPlaneHandle(pid)) {
    plane->kill();
    for (auto system : subsystems) system->killPlane(pid, *plane);
  }
}

void Sky::fireLaser(const PID pid) {
  if (auto *state = getPlane(pid)) {
    if (state->vital->requestDiscreteEnergy(0.3)) {
      appLog("PEW PEW");
    }
  }
}

void Sky::tick(float delta) {
  for (auto &elem : planes) {
    PlaneHandle &plane = elem.second;
    plane.writeToBody();
  }
  physics.tick(delta);
  for (auto &elem : planes) {
    PlaneHandle &plane = elem.second;
    plane.readFromBody();
    plane.tick(delta);
  }
  for (auto system : subsystems) system->tick(delta);
}

}
