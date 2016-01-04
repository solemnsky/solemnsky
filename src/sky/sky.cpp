#include "sky.h"

namespace sky {

Physics::Settings mySettings() {
  Physics::Settings settings{};
  settings.velocityIterations = 7; // etc
  return settings;
};

Sky::Sky() : physics(mySettings()) { }

/****
 * Handling planes.
 */

void Sky::joinPlane(const PID pid, const PlaneTuning tuning) {
  planes.emplace(std::pair<int, Plane>(pid, Plane(this)));
}

void Sky::quitPlane(const PID pid) {
  planes.erase(pid);
}

Plane *Sky::getPlane(const PID pid) {
  if (planes.find(pid) != planes.end())
    return &planes.at(pid);
  else return nullptr;
}

/****
 * Simulating.
 */

void Sky::tick(float delta) {
  physics.tick(delta);
  for (auto elem : planes) {
    Plane &plane = elem.second;
    plane.readFromBody();
    plane.tick(delta);
    plane.writeToBody();
  }
}


void Sky::render(const sf::Vector2<float> &pos) {
  // first find our actual viewpoint
  for (auto elem : planes) {
    // stub
  }
}
}
