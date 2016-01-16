/**
 * Subsystem abstraction, representing a engine module that interacts with
 * engine events through a simpler listener interface.
 */
#ifndef SOLEMNSKY_SUBSYSTEM_H
#define SOLEMNSKY_SUBSYSTEM_H

#include "sky/flight.h"

namespace sky {
typedef int PID; // ID for elements in the game

namespace detail {

class Subsystem {
public:
  // these are all in the past-tense, called after the action has completed
  // in the engine: spawned and killed planes here are already spawned and
  // killed, etc.

  virtual void tick(const float delta) { }

  virtual void joinPlane(const PID pid, Plane *plane) { }

  virtual void quitPlane(const PID pid) { }

  virtual void spawnPlane(const PID pid, Plane *plane) { }

  virtual void killPlane(const PID pid, Plane *plane) { }
};

}
}

#endif //SOLEMNSKY_SUBSYSTEM_H
