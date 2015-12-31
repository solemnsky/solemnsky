/**
 * Our flight mechanics system possesses both a complete detachment from the
 * physical reality of flight and a small complexity, surfacing in an assortment
 * of strange variables and tuning constants. We delegate the simple collision
 * detection / resolution we require to Box2d.
 *
 * The Plane class manages the state and simulation of a customizable plane.
 */
#ifndef SOLEMNSKY_FLIGHT_H
#define SOLEMNSKY_FLIGHT_H

#include <Box2D/Box2D.h>
#include "physics.h"
#include "base/util.h"

namespace sky {

class Engine;

struct PlaneTuning {
  b2Vec2 hitbox;

  struct {
    float
        maxRot, // maximum velocity of rotation
        maxVelCruise, // maximum velocity when cruising
        maxVelThruster, // maximum velocity when thrusting
        threshold; // velocity threshold at which we switch flight status
  } flight;

  float throttleSize;

  struct {
    float throttleUp, throttleDown;
    float afterburner;
  } velInfluence;

  struct {
    float
        maxRot,
        maxVel,
        threshold;
  } stall;

  float speed;
};

/**
 * Simple data structure with the state of a player.
 */
struct PlaneState {
  b2Vec2 pos, vel;
  float32 rot, rotvel{0};

  bool stalled{false}, afterburner{false};
  b2Vec2 leftoverVel{0, 0};
  float speed, throttle;

  PlaneState(Physics *physics, const PlaneTuning tuning, b2Vec2 pos,
             float rot);
};

class Plane {
private:
  sky::Engine *engine;
  Physics *physics;

  b2Body *body;

  /*
   * simulation
   */
  friend class Engine;

  void writeToBody();
  void readFromBody();
  void tick(float d);

public:
  PlaneTuning tuning;
  Optional<PlaneState> state{}; // only when spawned

  explicit Plane(Engine *engine, const PlaneTuning tuning);
  Plane();
  // TODO: why does something in the code (box2d?) require a null ctor for Plane??

  void spawn(b2Vec2 pos, float rot);
  void kill();
};
}

#endif //SOLEMNSKY_FLIGHT_H
