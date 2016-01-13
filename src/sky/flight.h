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

  sf::Vector2f hitbox{120, 70};

  /**
   * When we're in a stall, flight is much simpler.
   */
  struct {
    float
        maxRotVel = 180, // how quickly we can turn, (deg / s)
        maxVel = 100, // our terminal velocity (px / s)
        thrust = 100, // thrust acceleration (ps / s^2)
        damping = 0.5; // how quickly we approach our terminal velocity

    float threshold; // the minimum airspeed that we need to enter flight
  } stall;

  /**
   * When we enter flight, things become significantly more complicated.
   * TODO: implement
   */
  struct {
    float
        maxRotVel = 100,
        cruise = 300,
        thrustCruise = 400;
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
 * Plain data structure with all the concrete (useful) game state of a plane.
 * Only exists when the plane is alive <=> is participating in the game
 * mechanics.
 *
 * This contains things which need to be synced over the network; however it
 * obviously requires delta compression.
 */
struct PlaneState {
  PlaneState(Physics *physics, const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);
  PlaneTuning tuning;

  Clamped rotCtrl;
  Switched throtCtrl;

  sf::Vector2f pos, vel; // physical values
  float rot, rotvel;

  bool stalled; // flight mechanics
  sf::Vector2f leftoverVel;
  float speed;
  Clamped throttle;


  float forwardVelocity();
  float velocity();
};

/**
 * Plain data structure with all the animation state of a plane, alive or not.
 * This is derived from the PlaneState over time after having been thrust
 * into existence at the joining of a plane into the sky. It is is not necessary
 * to sync this over the network.
 *
 * Servers obviously don't have to simulate it; the only way for an end user
 * to cause a modification to this is to render the sky with a RenderMan.
 */
struct PlaneAnimState {
  PlaneAnimState() = default;

  bool afterburner{false};

  Switched orientation{{-1, 1}, 1}; // display
  Angle roll{0};

  // death animation could also go here, for example
};

class Plane {
private:
  sky::Sky *engine;
  Physics *physics;

  b2Body *body;

  /*
   * Simulation.
   */
  friend class Sky;

  friend class RenderMan;

  void writeToBody();
  void readFromBody();
  void tick(float d);
  void tickAnim(float d);

public:
  Plane(Sky *engine);
  ~Plane();
  Plane(Plane &&) = delete;
  Plane &operator=(Plane &&) = delete;
  Plane &operator=(const Plane &) = delete;

  // **you can't copy or move this**
  // because it carries a b2Body*, which is a bit of a problematic resource.
  // also it just doesn't make sense to move it; you have the base state
  // datastructures to access if you want

  /*
   * End-user API.
   */
  PlaneAnimState animState{};
  optional<PlaneState> state{}; // exists <=> plane is spawned

  void spawn(const sf::Vector2f pos, const float rot, const PlaneTuning &);
  void kill();
};
}

#endif //SOLEMNSKY_FLIGHT_H
