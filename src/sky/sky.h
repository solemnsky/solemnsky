/**
 * A sky (a solemn one) and all that it might hold, along with ways to draw it,
 * serialise it, simulate it, predict it, etc. Large general engine to be used
 * by the demos / clients / servers.
 */
#ifndef SOLEMNSKY_SKY_H
#define SOLEMNSKY_SKY_H

#include <map>
#include "physics.h"
#include "flight.h"
#include <memory>

namespace sky {

typedef int PID; // personal ID for elements in the game

class Sky {
private:
  std::map<PID, std::unique_ptr<Plane>> planes;

public:
  Sky();

  Physics physics;

  /**
   * Handling planes.
   */
  Plane * joinPlane(const PID pid, const PlaneTuning tuning);
  void quitPlane(const PID pid);
  Plane * getPlane(const PID pid);

  /**
   * Simulating.
   */
  void tick(float delta); // delta in seconds, as always
};
}

#endif //SOLEMNSKY_SKY_H
