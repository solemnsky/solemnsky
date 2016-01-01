#include <base/physics.h>
#include "engine.h"

namespace sky {

void Engine::joinPlane(PID pid, const PlaneTuning tuning) {
  planes[pid] = Plane(this, tuning);
}

void Engine::quitPlane(PID pid) {
  planes.erase(pid);
}

Plane &Engine::getPlane(PID pid) {
  return planes[pid];
}

void Engine::tick(float delta) {
  physics.tick(delta);
  for (auto elem : planes) {
    Plane &plane = elem.second;
    plane.readFromBody();
    plane.tick(delta);
    plane.writeToBody();
  }
}

/****
 * Physics settings.
 */

Physics::Settings mySettings() {
  Physics::Settings settings{};
  settings.velocityIterations = 7; // etc
  return settings;
};

Engine::Engine() : physics(mySettings()) {
  appLog(LogType::Notice, "Starting engine..."); // more important logging
}

Engine::~Engine() {
  appLog(LogType::Notice, "Destroying engine...");
}
}
