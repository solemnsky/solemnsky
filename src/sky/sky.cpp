#include "sky.h"

namespace sky {

Sky::Sky(const Map &map) : map(map), physics(map) {
  CTOR_LOG("sky");
}

Sky::~Sky() {
  DTOR_LOG("sky");
  planes.clear(); // destroy the planes before destroying the physics!
}

/****
 * Linking subsystems to the engine.
 */

void Sky::linkSystem(Subsystem *subsystem) {
  subsystems.push_back(subsystem);
}

/****
 * Handling planes.
 */

Plane *Sky::joinPlane(const PID pid) {
  planes[pid] = std::make_unique<PlaneHandle>(this);
  PlaneHandle *plane = planes[pid].get();
  for (auto system : subsystems) system->joinPlane(pid, plane);
  return &plane->state;
}

Plane *Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end()) return &planes[pid]->state;
  else return nullptr;
}

PlaneHandle *Sky::getPlaneHandle(const PID pid) {
  if (planes.find(pid) != planes.end()) return planes[pid].get();
  else return nullptr;
}

void Sky::quitPlane(const PID pid) {
  for (auto system : subsystems) system->quitPlane(pid);
  planes.erase(pid);
}

void Sky::spawnPlane(const PID pid, const sf::Vector2f pos, const float rot,
                     const PlaneTuning &tuning) {
  if (auto *plane = getPlaneHandle(pid)) {
    plane->spawn(pos, rot, tuning);
    for (auto system : subsystems) system->spawnPlane(pid, plane);
  }
}

void Sky::killPlane(const PID pid) {
  if (auto *plane = getPlaneHandle(pid)) {
    plane->kill();
    for (auto system : subsystems) system->killPlane(pid, plane);
  }
}

/****
 * Laser guns, cool right?
 */

void Sky::fireLaser(const PID pid) {
  if (auto *state = getPlane(pid)) {
    if (state->vital->requestDiscreteEnergy(0.3)) {
      appLog(LogType::Debug, "PEW PEW");
    }
  }
}

/****
 * Simulating.
 */

void Sky::tick(float delta) {

  for (auto &elem : planes) {
    PlaneHandle &plane = *elem.second;
    plane.writeToBody();
  }
  physics.tick(delta);
  for (auto &elem : planes) {
    PlaneHandle &plane = *elem.second;
    plane.readFromBody();
    plane.tick(delta);
  }
  for (auto system : subsystems) system->tick(delta);
}

}
