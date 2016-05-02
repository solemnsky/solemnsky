/**
 * Physical game state of an Arena.
 */
#pragma once
#include <map>
#include <memory>
#include "physics.h"
#include "plane.h"
#include "arena.h"

namespace sky {

/**
 * Initializer for the Sky Networked impl.
 */
struct SkyInitializer: public VerifyStructure {
  SkyInitializer() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(planes);
  }

  bool verifyStructure() const;

  std::map<PID, PlaneInitializer> planes; // planes already in the arena
};

/**
 * Delta for Sky Networked impl.
 */
struct SkyDelta: public VerifyStructure {
  SkyDelta() = default; // packing

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(planes);
  }

  bool verifyStructure() const;

  std::map<PID, PlaneDelta> planes;
};

/**
 * A Sky is a subsystem holding the potential state of a game being played.
 */
class Sky: public Subsystem, public PhysicsListener {
 private:
  friend struct Plane;
  std::map<PID, Plane> planes;
  Plane *planeFromPID(const PID pid);

 protected:
  void registerPlayerWith(Player &player, const PlaneInitializer &initializer);

  /**
   * Subsystem implementation.
   */
  void registerPlayer(Player &player) override;
  void unregisterPlayer(Player &player) override;

  void onTick(const float delta) override;
  void onJoin(Player &player) override;
  void onQuit(Player &player) override;
  void onMode(const ArenaMode newMode) override;
  void onMapChange() override;
  void onAction(Player &player, const Action action, const bool state) override;
  void onSpawn(Player &player, const PlaneTuning &tuning,
               const sf::Vector2f &pos, const float rot) override;

  // starting and stopping
  void stop();
  void start();

  // physics listeners
  virtual void onBeginContact(const BodyTag &body1,
                              const BodyTag &body2) override final;
  virtual void onEndContact(const BodyTag &body1,
                            const BodyTag &body2) override final;

 public:
  Sky(class Arena &parent, const SkyInitializer &initializer);
  ~Sky();

  optional<Map> map;
  optional<Physics> physics;
  // instantiated when the sky is running
  // invariant: this is the case when arena.mode == ArenaMode::Game
  // (guaranteed by constructor and onMode() subsystem callback)

  /**
   * API for the user.
   */
  // networking
  SkyInitializer captureInitializer();
  SkyDelta collectDelta();
  void applyDelta(const SkyDelta &delta);
  Plane &getPlane(const Player &player) const;

  // restart, resetting state
  void restart();
};

}
