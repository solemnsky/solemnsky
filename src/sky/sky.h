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

  std::map<PID, SkyPlayerInit> planes; // participations already in the arena
};

/**
 * Delta for Sky Networked impl.
 */
struct SkyDelta: public VerifyStructure {
  SkyDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(planes);
  }

  bool verifyStructure() const;

  std::map<PID, SkyPlayerDelta> planes;
};

/**
 * Game world when a game is in session.
 */
class Sky: public PhysicsListener {
  friend class SkyManager;
  friend class Participation;
 private:
  // Parameters.
  const Arena &arena;

  // State.
  Map map;
  Physics physics;
  std::map<PID, optional<Participation>> &skyPlayers;

 protected:
  // Internal API.
  void onTick(const float delta);

  // Physics listeners.
  virtual void onBeginContact(const BodyTag &body1,
                              const BodyTag &body2) override final;
  virtual void onEndContact(const BodyTag &body1,
                            const BodyTag &body2) override final;

 public:
  Sky(Arena &&arena, std::map<PID, optional<Participation>> &) = delete;
  Sky(const Arena &arena,
      std::map<PID, optional<Participation>> &skyPlayers);

};

/**
 * Wraps an optional<Sky>, binding it to the arena when the game is in session.
 */
class SkyManager: public Subsystem {
 private:
  // State.
  optional<Sky> sky;
  std::map<PID, optional<Participation>> participations;

 protected:
  // Internal help.
  void registerPlayerWith(Player &player, const SkyPlayerInit &initializer);
  optional<Participation> &accessParticipation(const Player &player);

  // Subsystem impl.
  void registerPlayer(Player &player) override;
  void unregisterPlayer(Player &player) override;

  void onTick(const float delta) override;
  void onJoin(Player &player) override;
  void onQuit(Player &player) override;
  void onMode(const ArenaMode newMode) override;
  void onMapChange() override;
  void onAction(Player &player,
                const Action action,
                const bool state) override;
  void onSpawn(Player &player,
               const PlaneTuning &tuning,
               const sf::Vector2f &pos,
               const float rot) override;

  // Starting / stopping.
  void stop();
  void start();

 public:
  SkyManager(class Arena &parent, const SkyInitializer &initializer);
  ~SkyManager();

  // Networking.
  SkyInitializer captureInitializer();
  SkyDelta collectDelta();
  void applyDelta(const SkyDelta &delta);

  // User API.
  void restart();
  const optional<Sky> &getSky() const;
  bool isActive() const;
  const optional<Participation> &getParticipation(const Player &player) const;

};

}
