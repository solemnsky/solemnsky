/**
 * A player's participation in a Sky.
 */
#pragma once
#include <Box2D/Box2D.h>
#include <forward_list>
#include "prop.h"
#include "physics.h"
#include "util/types.h"
#include "planestate.h"

namespace sky {

class SkyManager;

/**
 * Initializer for Participation's Networked impl.
 */
struct ParticipationInit {
  ParticipationInit();
  ParticipationInit(const PlaneControls &controls,
                const PlaneTuning &tuning,
                const PlaneState &state);
  ParticipationInit(const PlaneControls &controls);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(spawn, controls);
  }

  optional<std::pair<PlaneTuning, PlaneState>> spawn;
  PlaneControls controls;
};

/**
 * Delta for Participation's Networked impl.
 */
struct ParticipationDelta: public VerifyStructure {
  ParticipationDelta();

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
  friend class Sky;
  friend class Participation;
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

  // User API.
  const PlaneTuning &getTuning() const;
  const PlaneState &getState() const;

};

/**
 * A Player's participation in an active game.
 */
class Participation: public Networked<ParticipationInit, ParticipationDelta> {
  friend class Sky;
  friend class SkyManager;
 private:
  // Parameters.
  Physics &physics;

  // State.
  optional<Plane> plane;
  PlaneControls controls;
  std::forward_list<Prop> props;
  bool newlyAlive;

  // Helpers.
  void spawnWithState(const PlaneTuning &tuning,
                      const PlaneState &state);

  // Sky API.
  void doAction(const Action action, bool actionState);
  void prePhysics();
  void postPhysics(const float delta);

  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

 public:
  Participation() = delete;
  Participation(Physics &physics, const ParticipationInit &initializer);

  // Networked impl.
  void applyDelta(const ParticipationDelta &delta) override;
  ParticipationInit captureInitializer() const override;
  ParticipationDelta captureDelta();

  // User API.
  const optional<Plane> &getPlane() const;
  const std::forward_list<Prop> &getProps() const;
  const PlaneControls &getControls() const;
  bool isSpawned() const;

};

}