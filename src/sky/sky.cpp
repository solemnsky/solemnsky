#include "sky.h"

namespace sky {

Sky::Sky(const sf::Vector2f &dims) :
    physics(dims) {
  LIFE_LOG("Creating sky.");
}

Sky::~Sky() {
  LIFE_LOG("Destroying sky.");
  planes.clear(); // destroy the planes before destroying the physics!
}

/****
 * Handling planes.
 */

Plane *Sky::joinPlane(const PID pid, const PlaneTuning tuning) {
  planes.emplace(
      std::pair<int, std::unique_ptr<Plane>>(
          pid, std::unique_ptr<Plane>(new Plane(this))));
  return planes[pid].get();
}

void Sky::quitPlane(const PID pid) {
  planes.erase(pid);
}

Plane *Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end()) return planes[pid].get();
  else return nullptr;
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
}

}
