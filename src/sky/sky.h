/**
 * A Sky is the basic core of the game state, with a simple interface for
 * joining / quitting players, spawning and killing, etc. It is shared by
 * client and server. Non-universal subsystems such as graphics and delta
 * compression can be attached to a Sky with the subsystem framework.
 */
#ifndef SOLEMNSKY_SKY_H
#define SOLEMNSKY_SKY_H

#include <map>
#include "physics.h"
#include "flight.h"
#include <memory>

namespace sky {

typedef int PID; // ID for elements in the game

/**
 * A subsystem can listens for events in the engine.
 */
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

class Sky {
private:
  std::vector<Subsystem *> subsystems;

public:
  Sky(const sf::Vector2f &dims);
  void attachSystem(Subsystem *system);
  ~Sky();

  Physics physics;

  /**
   * Planes.
   */
  std::map<PID, std::unique_ptr<Plane>> planes;
  Plane *joinPlane(const PID pid, const PlaneTuning tuning);
  Plane *getPlane(const PID pid);
  void quitPlane(const PID pid);
  void spawnPlane(const PID pid, const sf::Vector2f pos, const float rot,
                  const PlaneTuning &tuning);
  void killPlane(const PID pid);

  /**
   * Laser guns: soon.
   */

  /**
   * Simulating.
   */
  void tick(float delta); // delta in seconds *everywhere*
};
}

#endif //SOLEMNSKY_SKY_H
