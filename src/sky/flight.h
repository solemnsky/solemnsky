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

class Sky;

struct PlaneTuning {
  sf::Vector2f hitbox;

  // TODO: sensible naming scheme?

  struct {
    float
        maxRot, // maximum velocity of rotation
        maxVelCruise, // maximum velocity when cruising
        maxVelThruster, // maximum velocity when thrusting
        threshold, // velocity threshold at which we switch flight status
        leftoverVelDamping; // rate at which the leftover vel is damped per s.
  } flight;

  float throttleSize;

  struct {
    float throttleUp, throttleDown;
    float afterburner;
    float throttleInfluence;
  } velInfluence;

  struct {
    float
        maxRot,
        maxVel,
        maxThrust,
        damping,
        threshold;
  } stall;

  float speed;
};

/**
 * Simple data structure with the state of a player.
 */
struct PlaneState {
  // controls: how we tell the plane where to fly
  Clamped rotCtrl{-1, -1} = 0; // rotation control
  Switched throtCtrl{{-1, 0, 1}} = 0; // throttle control

  // physical values
  sf::Vector2f pos, vel;
  float rot, rotvel{0};

  // flight mechanics values... this is a bit funny
  bool stalled{false}, afterburner{false};
  sf::Vector2f leftoverVel{0, 0};
  float speed, throttle;

  PlaneState(Physics *physics, const PlaneTuning tuning, const sf::Vector2f pos,
             const float rot);

  float forwardVelocity();
  float velocity();
};

class Plane {
private:
  sky::Sky *engine;
  Physics *physics;

  b2Body *body;

  /*
   * simulation
   */
  friend class Sky;

  void writeToBody();
  void readFromBody();
  void tick(float d);

public:
  PlaneTuning tuning;
  optional<PlaneState> state{}; // only when spawned

  Plane(Sky *engine, const PlaneTuning tuning);
  ~Plane();

  void spawn(const sf::Vector2f pos, const float rot);
  void kill();
};
}

#endif //SOLEMNSKY_FLIGHT_H
