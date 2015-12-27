/**
 * Central game engine, written to be used as a tool by the game tutorials
 * / clients / servers. Flight mechanics, physics helpers, network utilities...
 */
#ifndef SOLEMNSKY_ENGINE_ENGINE_H
#define SOLEMNSKY_ENGINE_ENGINE_H

#include <Box2D/Dynamics/b2World.h>
#include "physics.h"
#include "flight.h"
#include <map>

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

class Engine;

class Player {
public:
  Engine *engine;
  bool alive;
  sky::Plane plane;

  void kill();
  void spawn(const sky::PlaneState);
};

/**
 * Top-level manager for our entire game system.
 */
class Engine {
private:

public:
  Physics physics;

  std::map<PID, Plane> planes;
};
}

#endif //SOLEMNSKY_ENGINE_ENGINE_H
