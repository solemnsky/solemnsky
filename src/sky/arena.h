/**
 * The backbone of a multiplayer game.
 */
#pragma once
#include "util/types.h"
#include "util/methods.h"
#include "plane.h"
#include <map>
#include <list>
#include <vector>

struct ArenaEvent; // event.h

namespace sky {

/**
 * Player.
 */

struct PlayerInitializer {
  PlayerInitializer() = default; // packing
  PlayerInitializer(const PID pid, const std::string &nickname);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, pid, admin, team);
  }

  PID pid;
  std::string nickname;
  bool admin;
  Team team;
};

struct PlayerDelta {
  PlayerDelta() = default; // packing

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, admin, team);
  }

  optional<std::string> nickname;
  bool admin;
  optional<Team> team; // 0 is spectator, 1 left, 2 right
};

struct Player: public Networked<PlayerInitializer, PlayerDelta> {
 private:
  class Arena &arena;
 public:
  Player() = delete;
  Player(Arena &arena, const PlayerInitializer &initializer);

  const PID pid;
  std::string nickname;
  bool admin;
  Team team;
  std::vector<void *> data;

  // network API
  void applyDelta(const PlayerDelta &delta) override;
  PlayerInitializer captureInitializer() const override;
  PlayerDelta zeroDelta() const;

  // game API
  void doAction(const Action action, const bool state);
  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f &pos, const float rot);
};

/**
 * Subsystem.
 */

/**
 * Interface to call certain subsystem callbacks.
 */
class SubsystemCaller {
 private:
  class Arena &arena;

 public:
  SubsystemCaller(Arena &arena);

  // callbacks that subsystems can trigger
  void kill(Player &player);
  // TODO: takeDamage, etc
};

class Subsystem {
 protected:
  friend class Arena;
  friend class SubsystemCaller;
  const PID id; // ID the render has allocated in the Arena

  template<typename Data>
  Data &getPlayerData(const Player &player) const {
    return *((Data *) player.data.at(id));
  }

  // managing player registration: obligatory
  virtual void registerPlayer(Player &player) = 0;
  virtual void unregisterPlayer(Player &player) = 0;

  // arena-triggered callbacks
  virtual void onTick(const float delta) { }
  virtual void onJoin(Player &player) { }
  virtual void onQuit(Player &player) { }
  virtual void onAction(Player &player,
                        const Action action, const bool state) { }
  virtual void onSpawn(Player &player, const PlaneTuning &tuning,
                       const sf::Vector2f &pos, const float rot) { };
  virtual void onEvent(const ArenaEvent &event) { }

  // subsystem-triggered callbacks
  virtual void onKill(Player &player) { }

 public:
  class Arena &arena;
  SubsystemCaller caller;

  Subsystem() = delete;
  Subsystem(Arena &arena);
};

/**
 * Arena.
 */

struct ArenaInitializer {
  ArenaInitializer() = default; // packing
  ArenaInitializer(const std::string &name);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(players, name, motd, mode);
  }

  std::map<PID, PlayerInitializer> players;
  std::string name, motd;
  ArenaMode mode;
};

struct ArenaDelta: public VerifyStructure {
  enum class Type {
    Quit, Join, Delta, Motd, Mode
  };

  ArenaDelta() = default; // packing
  ArenaDelta(const Type type);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Quit: {
        ar(quit);
        break;
      }
      case Type::Join: {
        ar(join);
        break;
      }
      case Type::Delta: {
        ar(delta);
        break;
      }
      case Type::Motd: {
        ar(motd);
        break;
      }
      case Type::Mode: {
        ar(mode);
        break;
      }
    }
  }

  Type type;
  optional<PID> quit;
  optional<PlayerInitializer> join;
  optional<std::pair<PID, PlayerDelta>> delta;
  optional<std::string> motd;
  optional<ArenaMode> mode;

  bool verifyStructure() const override;

  static ArenaDelta Quit(const PID pid);
  static ArenaDelta Join(const PlayerInitializer &initializer);
  static ArenaDelta Delta(const PID, const PlayerDelta &delta);
  static ArenaDelta Motd(const std::string &motd);
  static ArenaDelta Mode(const ArenaMode mode);
};

class Arena: public Networked<ArenaInitializer, ArenaDelta> {
 private:
  // utilities
  PID allocPid() const;
  std::string allocNickname(const std::string &requested) const;

  friend class Player;
  friend class SubsystemCaller;

  // subsystem triggers, for Arena / Player to call
  void onAction(Player &player, const Action action, const bool state);
  void onSpawn(Player &player, const PlaneTuning &tuning,
               const sf::Vector2f &pos, const float rot);
  void onEvent(const ArenaEvent &event) const;

  // subsystem triggers, for subsystems to call
  void onDie(Player &player);

 public:
  Arena() = delete;
  Arena(const ArenaInitializer &initializer);

  // subsystems
  SubsystemCaller caller;
  std::vector<Subsystem *> subsystems;

  /**
   * State.
   */
  std::map<PID, Player> players;
  std::string name;
  std::string motd;
  ArenaMode mode;

  /**
   * User API.
   */
  void applyDelta(const ArenaDelta &delta) override;
  ArenaInitializer captureInitializer() const override;

  Player &joinPlayer(const PlayerInitializer &initializer);
  void quitPlayer(Player &player);
  Player &connectPlayer(const std::string &requestedNick);
  Player *getPlayer(const PID pid);
  void forPlayers(std::function<void(const Player &)> f) const;
  void forPlayers(std::function<void(Player &)> f);

  void tick(const float delta);
};

}
