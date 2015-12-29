#include "engine.h"

namespace sky {

void Engine::joinPlane(PID pid, const PlaneTuning tuning) {
  std::map<int, int> x;
  x[1] = 1;

  planes[pid] = Plane(*this, tuning);
}

void Engine::quitPlane(PID pid) {
  planes.erase(pid);
}

Plane *Engine::getPlane(PID pid) {
  return &planes[pid];
//  return nullptr;
}

}
