/**
 * The state and logic of Planes.
 */
#pragma once
#include <Box2D/Box2D.h>
#include <bitset>
#include <forward_list>
#include "prop.h"
#include "physics.h"
#include "util/types.h"
#include "planestate.h"

namespace sky {

class SkyHolder;

/**
 * Initializer for SkyPlayer's Networked implementation.
 */
struct SkyPlayerInit {
  SkyPlayerInit();
  SkyPlayerInit(const PlaneControls &controls,
                const PlaneTuning &tuning,
                const PlaneState &state);
  SkyPlayerInit(const PlaneControls &controls);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(spawn, controls);
  }

  optional<std::pair<PlaneTuning, PlaneState>> spawn;
  PlaneControls controls;
};

/**
 * Delta for SkyPlayer's Networked implementation.
 */
struct SkyPlayerDelta: public VerifyStructure {
  SkyPlayerDelta();

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(tuning, state, controls);
  }

  bool verifyStructure() const;

  optional<PlaneTuning> tuning; // if the plane spawned
  optional<PlaneState> state; // if the plane is alive
  optional<PlaneControls> controls; // if the controls changed
};

/**
 * The plane element that can be associated with a participation.
 */
class Plane {
  friend class SkyPlayer;
 private:
  // Parameters.
  Physics &physics;
  const PlaneControls &controls;

  // State.
  PlaneTuning tuning;
  PlaneState state;
  b2Body *const body;

  // Subroutines.
  void switchStall();
  void tickFlight(const float delta);
  void tickWeapons(const float delta);
  void writeToBody();
  void readFromBody();

  // Sky API.
  void prePhysics();
  void postPhysics(const float delta);
  void onBeginContact(const BodyTag &body);
  void onEndContact(const BodyTag &body);

 public:
  Plane() = delete;
  Plane(Physics &, PlaneControls &&, const PlaneTuning &,
        const PlaneState &) = delete; // `controls` must not be a temp
  Plane(Physics &physics,
        const PlaneControls &controls,
        const PlaneTuning &tuning,
        const PlaneState &state);
  ~Plane();

};

/**
 * A Player's participation in an active game.
 */
class SkyPlayer: public Networked<SkyPlayerInit, SkyPlayerDelta> {
  friend class Sky;
 private:
  // Parameters.
  Physics &physics;

  // State.
  optional<Plane> plane;
  PlaneControls controls;
  std::forward_list<Prop> props;
  bool newlyAlive;

  // Sky API.
  void doAction(const Action action, bool actionState);
  void prePhysics();
  void postPhysics(const float delta);

 public:
  SkyPlayer() = delete;
  SkyPlayer(Physics &physics, const SkyPlayerInit &initializer);

  // Networked impl.
  void applyDelta(const SkyPlayerDelta &delta) override;
  SkyPlayerInit captureInitializer() const override;
  SkyPlayerDelta captureDelta();

  // User API.
  void spawnWithState(const PlaneTuning &tuning,
                      const PlaneState &state);
  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

  const optional<Plane> &getPlane() const;
  const std::forward_list<Prop> &getProps() const;
  const PlaneControls &getControls() const;
  bool isSpawned() const;

};

}
