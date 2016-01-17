#include "sky.h"

namespace sky {

Sky::Sky(const sf::Vector2f &dims) : physics(dims) {
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

Plane *Sky::joinPlane(const PID pid, const PlaneTuning tuning) {
  planes[pid] = std::make_unique<Plane>(this);
  Plane *plane = planes[pid].get();
  for (auto system : subsystems) system->joinPlane(pid, plane);
  return plane;
}

Plane *Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end()) return planes[pid].get();
  else return nullptr;
}

void Sky::quitPlane(const PID pid) {
  for (auto system : subsystems) system->quitPlane(pid);
  planes.erase(pid);
}

void Sky::spawnPlane(const PID pid, const sf::Vector2f pos, const float rot,
                     const PlaneTuning &tuning) {
  if (Plane *plane = getPlane(pid)) {
    plane->spawn(pos, rot, tuning);
    for (auto system : subsystems) system->spawnPlane(pid, plane);
  }
}

void Sky::killPlane(const PID pid) {
  if (Plane *plane = getPlane(pid)) {
    plane->kill();
    for (auto system : subsystems) system->killPlane(pid, plane);
  }
}

/****
 * Simulating.
 */

void Sky::tick(float delta) {
  for (auto &elem : planes) {
    Plane &plane = *elem.second;
    plane.writeToBody();
  }
  physics.tick(delta);
  for (auto &elem : planes) {
    Plane &plane = *elem.second;
    plane.readFromBody();
    plane.tick(delta);
  }
  for (auto system : subsystems) system->tick(delta);
}

}
