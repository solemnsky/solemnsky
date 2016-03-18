/**
 * Core game state. The sky, really. Subordinate to Arena.
 */
#pragma once
#include <map>
#include <memory>
#include "physics.h"
#include "flight.h"
#include "map.h"

namespace sky {

typedef unsigned int PID; // ID for elements in the game

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
  virtual void addPlane(const PID pid, Plane &plane) { }

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
    ar(restructure, state);
  }

  // we potentially remove / [re]add Planes
  std::map<PID, optional<PlaneInitializer>> restructure;
  // we set the state of planes
  std::map<PID, PlaneState> state;
};

/*
 * A Sky is the basic core of the game state. It is semantically subordinate to
 * an Arena, and exposes a simple interface for clients and servers alike for
 * all kinds of circumstances, from server-side simulation to replaying
 * recordings.
 *
 * Planes are added and removed to a Sky "all willy-nilly"; holding
 * persistent player records and such is the task of the Arena.
 */
class Sky {
 private:
  std::vector<Subsystem *> subsystems;

  std::map<PID, Plane> planes;

  // This keeps track of PID values where planes are removed / added.
  // `restructure.at(pid) == nullptr` <=> At pid, there is no longer any Plane.
  // Otherwise, restructure.at(pid) points to a plane that was recently added.
  // Reset at each collectDelta.
  std::map<PID, Plane *> restructure;

 public:
  Sky(const MapName &mapName);
  Sky(const SkyInitializer &initializer);
  ~Sky();

  const MapName mapName;
  const Map map; // physics uses this at construction
  Physics physics;

  /**
   * Linking subsystems to the engine.
   */
  void linkSystem(Subsystem *);

  /**
   * Interface.
   */
  Plane *getPlane(const PID pid);
  Plane &addPlane(const PID pid,
                  const PlaneTuning &tuning,
                  const sf::Vector2f pos,
                  const float rot);
  void removePlane(const PID pid);

  void fireLaser(Plane *const plane);

  void tick(float delta); // delta in seconds *everywhere*

  /**
   * Initializers and deltas.
   */
  SkyInitializer captureInitializer();
  SkyDelta collectDelta();
  void applyDelta(const SkyDelta &delta);
};

}
