/**
 * The backbone of a multiplayer game.
 */
#pragma once
#include "util/types.h"
#include "event.h"
#include <map>
#include <list>
#include <vector>

namespace sky {

/**
 * Player.
 */

struct PlayerInitializer {
 private:
  friend cereal::access;
  friend class Player;
  PlayerInitializer() = default;

 public:
  PlayerInitializer(const std::string &nickname);

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
 private:
  friend cereal::access;
  friend class Player;
  PlayerDelta() = default;

 public:
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, admin, team);
  }

  optional<std::string> nickname;
  bool admin;
  optional<Team> team; // 0 is spectator, 1 left, 2 right
};

struct Player: public Networked<PlayerInitializer, PlayerDelta> {
  Player() = delete;
  Player(const PlayerInitializer &initializer);

  const PID pid;
  std::string nickname;
  bool admin;
  Team team;
  std::vector<void *> data;

  void applyDelta(const PlayerDelta &delta) override;
  PlayerInitializer captureInitializer() const override;
  PlayerDelta zeroDelta() const;
};

/**
 * Subsystem.
 */

class Subsystem {
 private:
  template<typename Data>
  Data getPlayerData(Player &player) const {
    return *((Data *) player.data[id]);
  }

 protected:
  friend class Arena;

  virtual void initialize(Player &player);
  virtual void tick(const float delta) { }
  virtual void join(Player &player) { }
  virtual void quit(Player &player) { }

  virtual void nickChange(Player &player, const std::string &oldNick) { }
  virtual void teamChange(Player &player, const Team oldTeam) { }

  class Arena *arena;
  const PID id; // ID the subsystem has allocated in the Arena

 public:
  Subsystem() = delete;
  Subsystem(Arena *arena);
};

/**
 * Arena.
 */

enum class ArenaMode {
  Lobby, // lobby, to make teams
  Game, // playing tutorial
  Scoring // viewing tutorial results
};

struct ArenaInitializer {
 private:
  friend cereal::access;
  friend class Arena;
  ArenaInitializer() = default;

 public:
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
 private:
  friend cereal::access;
  ArenaDelta() = default;
  ArenaDelta(const Type type);

 public:
  enum class Type {
    Quit, Join, Delta, Motd, Mode
  };

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
  static ArenaDelta Join(const PID pid, const Player &player);
  static ArenaDelta Delta(const PID, const PlayerDelta &delta);
  static ArenaDelta Motd(const std::string &motd);
  static ArenaDelta Mode(const ArenaMode mode);
};

class Arena: public Networked<ArenaInitializer, ArenaDelta> {
 private:
  // utilities
  PID allocPid() const;
  std::string allocNickname(const std::string &requested) const;

  // subsystems
  friend class Subsystem;
  std::vector<Subsystem *> subsystems;
  void forSubsystems(std::function<void(Subsystem &)> call);
  void forPlayers(std::function<void(Player &)> call);

 public:
  Arena() = delete;
  Arena(const ArenaInitializer &initializer);

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
};

}
