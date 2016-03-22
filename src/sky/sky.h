/**
 * Core game state. The sky, really. Subordinate to Arena.
 */
#pragma once
#include <map>
#include <memory>
#include "physics.h"
#include "plane.h"
#include "map.h"

namespace sky {

/**
 * Basic types.
 */
typedef unsigned int PID; // ID for elements in the game
typedef unsigned char Team; // 0 spec, 1 red, 2 blue

/**
 * Subsystem abstraction, representing a module that attaches to Sky events
 * through a simple callback interface.
 *
 * To use a subsystem, we construct a Sky first, and then construct the
 * subsystem with a pointer to the sky. Finally, to tie the subsystem into
 * sky's events, we link it in with Sky::link().
 *
 * > Sky sky({1600, 900});
 * > <Subsystem> someSubsystem(&sky);
 * > sky.link(render);
 */
class Subsystem {
 protected:
  const Sky *sky; // pointer to the sky it's attached to

 private:
  friend class Sky;

  // post-tick
  virtual void tick(const float delta) { }

  // post-add
  virtual void addPlane(const PID pid, PlaneVital &plane) { }

  // pre-remove
  virtual void removePlane(const PID pid) { }

 public:
  Subsystem(const Sky *sky) : sky(sky) { }

  // a Subsystem's life is never managed from a handle to a Subsystem, but
  // rather from non-virtual subclasses; we don't need a virtual dtor.
};

/**
 * The stuff we need to copy a sky over a connection.
 */
struct SkyInitializer {
  SkyInitializer();
  template<typename Archive>

  void serialize(Archive &ar) {
    ar(mapName, planes);
  }

  MapName mapName; // the map to load
  std::map<PID, PlaneInitializer> planes; // planes already in the arena
};

/**
 * Delta in a sky, like a snapshot but smarter.
 */
struct SkyDelta {
  SkyDelta();

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(state);
  }

  // we set the state of planes
  std::map<PID, PlaneDelta> state;
  // we set the state of the props
  // TODO
};

/*
 * A Sky is the state of a game being played. It is held by an arena when a
 * game is under way. Every Player corresponds to one Plane.
 */
class Sky {
 private:
  std::vector<Subsystem *> subsystems;
  std::map<PID, Plane> planes;

  /**
   * API for the container Arena.
   */
  Plane &addPlane(const PID pid);
  void removePlane(const PID pid);
  Plane *getPlane(const PID pid);

 public:
  Sky(const MapName &mapName);
  Sky(const SkyInitializer &initializer);
  ~Sky();

  const MapName mapName;
  const Map map; // physics uses this at construction
  Physics physics;

  /**
   * API for the user.
   */
  void linkSystem(Subsystem *);
  void tick(float delta); // delta in seconds *everywhere*

  SkyInitializer captureInitializer();
  SkyDelta collectDelta();
  void applyDelta(const SkyDelta &delta);
};

}
