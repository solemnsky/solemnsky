/**
 * A sky (a solemn one) and all that it might hold, along with ways to draw it,
 * serialise it, simulate it, predict it, etc. Large general engine to be used
 * by the demos / clients / servers.
 */
#ifndef SOLEMNSKY_ENGINE_ENGINE_H
#define SOLEMNSKY_ENGINE_ENGINE_H

#include <map>
#include "physics.h"
#include "flight.h"

namespace sky {

struct PlayerInput {
  const enum class Action {
    ThrottleUp, ThrottleDown, Left, Right, Primary, Secondary
  } action;

  const bool actionState;

  PlayerInput(Action action, bool actionState) : action(action),
                                                 actionState(actionState) { }
};

typedef int PID; // personal ID for elements in the game

/**
 * Top-level manager for our entire game system.
 */
class Sky {
private:
  std::map<PID, Plane> planes;

public:
  Physics physics;

  Sky();
  ~Sky();

  /**
   * Handling planes.
   */
  void joinPlane(const PID pid, const PlaneTuning tuning);
  void quitPlane(const PID pid);
  Plane *getPlane(const PID pid);

  /**
   * Simulating.
   */
  void handle(const PID pid, const PlayerInput);
  void tick(float delta); // delta in seconds, as always
};
}

#endif //SOLEMNSKY_ENGINE_ENGINE_H
