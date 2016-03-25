/**
 * The backbone of a multiplayer game.
 */
#pragma once
#include "util/types.h"
#include "util/methods.h"
#include <map>
#include <list>
#include <vector>

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
 protected:
  template<typename Data>
  Data &getPlayerData(const Player &player) {
    return *((Data *) player.data.at(id));
  }

  friend class Arena;

  // push a pointer onto the player's data
  virtual void registerPlayer(Player &player) = 0;
  // destroy the data the player was attached to
  virtual void unregisterPlayer(Player &player) = 0;

  virtual void tick(const float delta) { }
  virtual void join(Player &player) { }
  virtual void quit(Player &player) { }

  virtual void nickChange(Player &player, const std::string &oldNick) { }
  virtual void teamChange(Player &player, const Team oldTeam) { }

  class Arena *const arena;
  const PID id; // ID the subsystem has allocated in the Arena

 public:
  Subsystem() = delete;
  Subsystem(Arena *const arena);
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

  void tick(const float delta);
};

}
