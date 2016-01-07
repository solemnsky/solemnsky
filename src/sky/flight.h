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
#include "base/base.h"

namespace sky {

class Sky;

/**
 * Tuning values describing how a plane flies.
 * The flight system is fairly unrealistic but intuitive.
 * Altitude did this well, we're making a little twist on it.
 *
 * There are a lot of values and terms to juggle, documented below.
 */
struct PlaneTuning {
  PlaneTuning() { }

  sf::Vector2f hitbox{200, 200};

  /**
   * When we're in a stall, flight is much simpler.
   */
  struct {
    float
        maxRotVel, // how quickly we can turn, (rad / s)
        maxVel, // our terminal velocity (px / s)
        thrust, // thrust acceleration (ps / s^2)
        damping; // how quickly we approach our terminal velocity

    float threshold; // the minimum airspeed that we need to enter flight
  } stall;

  /**
   * When we enter flight, things become significantly more complicated.
   * TODO: implement
   */
  struct {
    float
        maxRotVel = 1, // maximum velocity of rotation
        cruise = 300, // max cruise velocity
        thrustCruise = 400; // max cruise velocity with thrusters engaged
//
//    float leftoverVelDamping = 0.7; // rate at which the leftover vel is damped
//
//    float throttleUp = 1, throttleDown = 1;
//    float afterburner = 20;
//    float throttleInfluence = 0.8;
//
//    float threshold = 60; // the maximum airspeed that we need to leave flight

  } flight;

  float throttleSize = 1;
};

/**
 * Plain data structure with the state of a player.
 */
struct PlaneState {
  PlaneState(Physics *physics, const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);
  /**
   * Tuning.
   */
  PlaneTuning tuning;

  /**
   * Control states.
   */
  Clamped rotCtrl; // rotation control
  Switched throtCtrl; // throttle control

  /**
   * Physical values and flight mechanics.
   */
  sf::Vector2f pos, vel;
  float rot, rotvel;

  bool stalled, afterburner;
  sf::Vector2f leftoverVel;
  float speed;
  Clamped throttle;

  /**
   * Derived values.
   */
  float forwardVelocity();
  float velocity();
};

class Plane {
private:
  sky::Sky *engine;
  Physics *physics;

  b2Body *body{nullptr};

  /*
   * Simulation.
   */
  friend class Sky;

  void writeToBody();
  void readFromBody();
  void tick(float d);

public:
  Plane(Sky *engine);
  ~Plane();
  Plane(Plane &&) = delete;
  Plane &operator=(Plane &&) = delete; // potentially tied to a b2Body

  /**
   * End interface.
   */
  optional<PlaneState> state{}; // exists <=> plane is spawned

  void spawn(const sf::Vector2f pos, const float rot, const PlaneTuning &);
  void kill();
};
}

#endif //SOLEMNSKY_FLIGHT_H
