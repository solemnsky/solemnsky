/**
 * A Sky is the basic core of the game state. It is semantically subordinate to
 * an Arena, and exposes a simple interface for clients and servers alike for
 * all kinds of circumstances, from server-side simulation to replaying
 * recordings.
 *
 * Non-universal subsystems such as graphics and delta compression can be
 * attached to a Sky with the 'Subsystem' framework.
 */
#ifndef SOLEMNSKY_SKY_H
#define SOLEMNSKY_SKY_H

#include <map>
#include <memory>
#include "physics.h"
#include "flight.h"
#include "subsystem.h"
#include "map.h"

namespace sky {

class Sky {
private:
  std::vector<Subsystem *> subsystems;

public:
  Sky(const Map &map);
  ~Sky();

  const Map map;
  Physics physics;

  /**
   * Linking subsystems to the engine.
   */
  void linkSystem(Subsystem *);

  /**
   * Planes.
   */
  std::map<PID, std::unique_ptr<PlaneHandle>> planes;
  Plane *joinPlane(const PID pid);
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
