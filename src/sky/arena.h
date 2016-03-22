/**
 * Shared model of a multiplayer game.
 */
#pragma once
#include "util/types.h"
#include "sky.h"
#include "event.h"
#include <map>
#include <list>
#include <vector>

namespace sky {

struct PlayerInitializer {
 private:
  friend cereal::access;
  friend class Player;
  PlayerInitializer() = default;

 public:
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(pid, nickname, admin, team);
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

/**
 * A Player represents everything related to a player in the game.
 */
struct Player: public Networked<PlayerInitializer, PlayerDelta> {
  Player();
  Player(const PID pid);
  Player(const PlayerInitializer &initializer);

  PID pid;
  std::string nickname;
  bool admin;
  Team team;

  Plane *plane;

  void applyDelta(const PlayerDelta &delta);
};

enum class ArenaMode {
  Lobby, // lobby, to make teams
  Game, // playing tutorial
  Scoring // viewing tutorial results
};

struct ArenaInitializer: public VerifyStructure {
  ArenaInitializer(); // for unpacking

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(skyInitializer, players, motd, mode);
  }

  optional<SkyInitializer> skyInitializer;
  std::vector<Player> players;
  std::string motd;
  ArenaMode mode;

  bool verifyStructure() const override;
};

struct ArenaDelta: public VerifyStructure {
  enum class Type {
    Quit, // a player quits
    Join, // a player joints
    Modify, // a player's data is modified
    Motd, // the motd changes
    Mode // the mode changes
  };

  ArenaDelta();
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
      case Type::Modify: {
        ar(player);
        break;
      }
      case Type::Motd: {
        ar(motd);
        break;
      }
      case Type::Mode: {
        ar(arenaMode, skyInitializer);
        break;
      }
    }
  }

  Type type;
  optional<PID> quit;
  optional<Player> join;
  optional<std::pair<PID, PlayerDelta>> player;
  optional<std::string> motd;
  optional<ArenaMode> arenaMode;
  optional<SkyInitializer> skyInitializer;

  bool verifyStructure() const override;

  static ArenaDelta Quit(const PID pid);
  static ArenaDelta Join(const Player &player);
  static ArenaDelta Modify(const PID, const PlayerDelta &pDelta);
  static ArenaDelta Motd(const std::string &motd);
  static ArenaDelta Mode(const ArenaMode mode,
                         const optional<SkyInitializer> &skyInitializer = {});
};

/**
 * A model of a game arena.
 * Used by both server and client.
 */
class Arena: public Networked<ArenaInitializer, ArenaDelta> {
 private:
  PID allocPid() const;
  std::string allocNickname(const std::string &requested) const;

  std::vector<Subsystem *> subsystems;

  // subsystem callbacks
  void join(const Player &player);
  void quit(const Player &player);

 public:
  Arena();

  /**
   * State.
   */
  std::list<Player> players;
  std::string motd;

  ArenaMode mode;
  Sky sky;

  /**
   * Subsystem.
   */
  void linkSystem(Subsystem *subsystem);

  /**
   * User API.
   */
  void applyInitializer(const ArenaInitializer &initializer);
  void applyDelta(const ArenaDelta &delta) override;
  ArenaInitializer captureInitializer();

  Player &connectPlayer(const std::string &requestedNick);
  void disconnectPlayer(const Player &record);
  Player *getPlayer(const PID pid);
};

}
