#include "sky.h"

namespace sky {

Physics::Settings mySettings() {
  Physics::Settings settings{};
  settings.velocityIterations = 7; // etc
  return settings;
};

Sky::Sky() : physics({1600, 900}, mySettings()) { }

/****
 * Handling planes.
 */

Plane &Sky::joinPlane(const PID pid, const PlaneTuning tuning) {
  planes.emplace(
      std::pair<int, std::unique_ptr<Plane>>(
          pid, std::unique_ptr<Plane>(new Plane(this))));
  return *planes[pid];
}

void Sky::quitPlane(const PID pid) {
  planes.erase(pid);
}

Plane &Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end()) return *planes[pid];
}

/****
 * Simulating.
 */

void Sky::tick(float delta) {
  physics.tick(delta);
  for (auto &elem : planes) {
    Plane &plane = *elem.second;
    plane.readFromBody();
    plane.tick(delta);
    plane.writeToBody();
  }
}
}
