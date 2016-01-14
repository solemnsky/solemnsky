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

  sf::Vector2f hitbox{110, 60}; // x axis parallel with flight

  struct {
    // mechanics when stalled
    float maxRotVel = 200, // how quickly we can turn, (deg / s)
        maxVel = 300, // our terminal velocity (px / s)
        thrust = 500, // thrust acceleration (ps / s^2)
        damping = 0.8; // how quickly we approach our terminal velocity
    float threshold = 130; // the minimum airspeed that we need to enter flight
  } stall;

  struct {
    // mechanics when not stalled
    float maxRotVel = 180,
        maxAirspeed = 330, //
        throttleInfluence = 0.6,
        throttleDrive = 0.3,
        throttleBreaking = 1.1,
        gravityEffect = 0.5,
        afterburnDrive = 0.9,
        leftoverDamping = 1.5;
    float threshold = 100; // the maximum airspeed that we need to enter stall
  } flight;

  float throttleSpeed = 1.5;
};

/**
 * Plain data structure with all the concrete game state of a plane.
 * Only held by Plane when it is alive <=> is participating in the game
 * mechanics.
 *
 * This contains things which need to be synced over the network; however it
 * obviously can take some serious delta compression.
 */
struct PlaneState {
  PlaneState(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

  PlaneTuning tuning;

  Clamped rotCtrl; // controls
  Switched throtCtrl;

  sf::Vector2f pos, vel; // physical values
  Angle rot;
  float rotvel;

  bool stalled, afterburner; // flight mechanics
  sf::Vector2f leftoverVel;
  Clamped airspeed;
  Clamped throttle;

  float forwardVelocity();
  float velocity();
};

/**
 * Plain data structure with all the animation state of a plane, alive or not.
 * This is derived from the PlaneState over time after having been thrust
 * into existence at the joining of a plane into the sky, reset to default
 * at each respawn. It is is not necessary * to sync this over the network.
 *
 * Servers obviously don't have to simulate it; the only way for an end user
 * to cause a modification to this is to render the sky with a RenderMan.
 */
struct PlaneAnimState {
  PlaneAnimState();

  Angle roll;
  bool orientation;

  float flipState; // these two values contribute to the roll
  float rollState;

  const static struct {
    // const static settings describing how animation happens
    // TODO: decide concretely if this deserves a place somewhere else?
    float rollAmount = 30, // how many degrees it rolls to either side
        rollSpeed = 2; // how quickly it rolls 2 * rollAmount
  } settings;

private:
  /*
   * Delegating some management from the Plane it belongs to:
   */
  friend class Plane;

  void tick(class Plane *parent, const float delta);
  void reset(); // when the plane respawns
  // death is animated in the a normal course of events, but respawning changes
  // everything anew
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
  void tickAnim(float delta);

public:
  Plane(Sky *engine);
  ~Plane();

  Plane(Plane &&) = delete;
  Plane &operator=(Plane &&) = delete;
  Plane &operator=(const Plane &) = delete;

  // **you can't copy or move this**
  // because it carries a b2Body*, which is a bit of a problematic resource.
  // also it just doesn't make sense to move it; you have the base state
  // data-structures to access if you want

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
