/**
 * Arena subsystem with game state.
 */
#pragma once
#include <map>
#include <memory>
#include "physics.h"
#include "plane.h"
#include "arena.h"

namespace sky {

struct SkyInitializer {
  SkyInitializer() = default; // packing
  SkyInitializer(const MapName &mapName);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(mapName, planes);
  }

  MapName mapName; // the map to load
  std::map<PID, PlaneInitializer> planes; // planes already in the arena
};

struct SkyDelta: public VerifyStructure {
  SkyDelta() = default; // packing

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(planes);
  }

  bool verifyStructure() const;

  std::map<PID, PlaneDelta> planes;
};

/*
 * A Sky is the state of a game being played. It is held by an Arena when a
 * game is under way.
 */
class Sky: public Subsystem {
 private:
  std::map<PID, Plane> planes;
  Plane *getPlane(const PID pid);

 protected:
  friend class Arena;

  /**
   * Subsystem implementation.
   */
  virtual void tick(const float delta) override;
  virtual void initialize(Player &player) override {
    Subsystem::initialize(player);
  }
  virtual void join(Player &player) override;
  virtual void quit(Player &player) override;

 public:
  Sky(class Arena *parent, const SkyInitializer &initializer);
  ~Sky();

  const MapName mapName;
  const Map map; // physics uses this at construction
  Physics physics;

  /**
   * API for the user.
   */
  SkyInitializer captureInitializer();
  SkyDelta collectDelta();
  void applyDelta(const SkyDelta &delta);
};

}
