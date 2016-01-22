/**
 * Data structures / simulation and box2d integration for planes.
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
 */
struct PlaneTuning {
  PlaneTuning() { }

  sf::Vector2f hitbox{110, 60}; // x axis parallel with flight
  float maxHealth = 10;

  struct {
    float thrustDrain = 1;
    float recharge = 0.5;
    float laserGun = 0.3;
  } energy;

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
        airspeedFactor = 330,
        throttleInfluence = 0.6,
        throttleDrive = 0.3,
        throttleBreaking = 1.1,
        gravityEffect = 0.6,
        afterburnDrive = 0.9,
        leftoverDamping = 0.3;
    float threshold = 110; // the maximum airspeed that we need to enter stall
  } flight;

  float throttleSpeed = 1.5;
};

/**
 * State specific to a plane that is spawned.
 */
struct PlaneVital {
  PlaneVital(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

  /**
   * Data.
   */
  PlaneTuning tuning;

  Clamped rotCtrl; // controls
  Switched throtCtrl;

  sf::Vector2f pos, vel; // physical values
  Angle rot;
  float rotvel;

  bool stalled; // flight mechanics
  Clamped afterburner;
  sf::Vector2f leftoverVel;
  Clamped airspeed, throttle;

  Clamped energy, health; // game mechanics

  /**
   * Helper methods.
   */
  float forwardVelocity();
  float velocity();

  // returns true if energy was drawn
  bool requestDiscreteEnergy(const float reqEnergy);
  // returns the fraction of the requested energy that was drawn
  float requestEnergy(const float reqEnergy);
};

/**
 * A plane, expressed in a simple (copyable etc.) struct.
 */
struct Plane {
  Plane();

  /**
   * Data.
   */
  std::string name;
  optional<PlaneVital> vital; // exists <=> plane is spawned

  /**
   * Helper methods.
   */
  // ???
};

/**
 * Handle for a
 */
class PlaneHandle {
private:
  sky::Sky *engine;
  Physics *physics;
  b2Body *body;

  /*
   * State mutation.
   */
  friend class Sky;

  void writeToBody();
  void readFromBody();
  void tick(float d);

  void spawn(const sf::Vector2f pos, const float rot, const PlaneTuning &);
  void kill();

public:
  PlaneHandle(Sky *engine);
  ~PlaneHandle();

  PlaneHandle(PlaneHandle &&) = delete;
  PlaneHandle &operator=(PlaneHandle &&) = delete;
  PlaneHandle &operator=(const PlaneHandle &) = delete;

  // **you can't copy or move this**
  // because it carries a b2Body*, which is a bit of a problematic resource.
  // also it just doesn't make sense to move it; you have the base state
  // data-structures to access if you want

  Plane state{}; // exists <=> plane is spawned
};
}

#endif //SOLEMNSKY_FLIGHT_H
