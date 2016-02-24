/**
 * The core game engine / game state manager, along with an abstraction for
 * building subsystems that link into callbacks and augment its functionality.
 */
#ifndef SOLEMNSKY_SKY_H
#define SOLEMNSKY_SKY_H

#include <map>
#include <memory>
#include "physics.h"
#include "flight.h"
#include "map.h"
#include "telegraph/pack.h"

namespace sky {

typedef unsigned int PID; // ID for elements in the game
const static tg::Pack<PID> pidPack = tg::BytePack<PID>();

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
  Sky *sky; // pointer to the sky it's attached to

 private:
  friend class Sky;

  // these are all in the past-tense, called after the action has completed
  // in the engine: spawned and killed planes here are already spawned and
  // killed, etc.
  virtual void tick(const float delta) { }

  virtual void joinPlane(const PID pid, PlaneHandle &plane) { }

  virtual void quitPlane(const PID pid) { }

  virtual void spawnPlane(const PID pid, PlaneHandle &plane) { }

  virtual void killPlane(const PID pid, PlaneHandle &plane) { }

 public:
  Subsystem(Sky *sky) : sky(sky) { }

  // a Subsystem's life is never managed from a handle to a Subsystem, but
  // rather from non-virtual subclasses; we don't need a virtual dtor.
};

/**
 * The stuff we need to recreate a Sky.
 */
struct SkyInitializer {
  MapName mapName; // the map to load
  std::map<PID, Plane> planes; // planes already in the arena
};

struct SkyInitializerPack: public tg::ClassPack<SkyInitializer> {
  SkyInitializerPack();
};

/**
 * Changes that happened in a sky.
 */
struct SkyDelta {

};

struct SkyDeltaPack: public tg::ClassPack<SkyDelta> {
  SkyDeltaPack();
};

/*
 * A Sky is the basic core of the game state. It is semantically subordinate to
 * an Arena, and exposes a simple interface for clients and servers alike for
 * all kinds of circumstances, from server-side simulation to replaying
 * recordings.
 */
class Sky {
 private:
  std::vector<Subsystem *> subsystems;

 public:
  Sky(const Map &map);
  Sky(const SkyInitializer &initializer);
  ~Sky();

  const Map map; // physics uses this at construction
  Physics physics;

  /**
   * Linking subsystems to the engine.
   */
  void linkSystem(Subsystem *);

  /**
   * Planes.
   */
  std::map<PID, PlaneHandle> planes;
  Plane &joinPlane(const PID pid);
  Plane *getPlane(const PID pid);
  PlaneHandle *getPlaneHandle(const PID pid);
  void quitPlane(const PID pid);
  void spawnPlane(const PID pid, const sf::Vector2f pos, const float rot,
                  const PlaneTuning &tuning);
  void killPlane(const PID pid);

  /**
   * Laser guns: now in a cinema near you.
   */
  void fireLaser(const PID pid);

  /**
   * Simulating.
   */
  void tick(float delta); // delta in seconds *everywhere*
};

}

#endif //SOLEMNSKY_SKY_H
