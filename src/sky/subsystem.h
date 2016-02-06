/**
 * Subsystem abstraction, representing a engine module that attaches to
 * engine events through a simple callback interface.
 *
 * Quick overview of what's going on:
 * To use a subsystem, we construct a sky first, and then construct the
 * subsystem with a pointer to the sky. Finally, to tie the subsystem into
 * sky's events, you to link it in with Sky::link().
 *
 * > Sky sky({1600, 900});
 * > Render render(&sky);
 * > sky.link(render);
 */
#ifndef SOLEMNSKY_SUBSYSTEM_H
#define SOLEMNSKY_SUBSYSTEM_H

#include "sky/flight.h"

namespace sky {
typedef int PID; // ID for elements in the game

class Subsystem {
protected:
  Sky *sky; // pointer to the sky it's attached to

private:
  friend class Sky;

  // these are all in the past-tense, called after the action has completed
  // in the engine: spawned and killed planes here are already spawned and
  // killed, etc.
  virtual void tick(const float delta) { }

  virtual void joinPlane(const PID pid, PlaneHandle *plane) { }

  virtual void quitPlane(const PID pid) { }

  virtual void spawnPlane(const PID pid, PlaneHandle *plane) { }

  virtual void killPlane(const PID pid, PlaneHandle *plane) { }

public:
  Subsystem(Sky *sky) : sky(sky) { }

  // a Subsystem's life is never managed from a handle to a Subsystem, but
  // rather from non-virtual subclasses; we don't need a virtual dtor.
};
}

#endif //SOLEMNSKY_SUBSYSTEM_H
