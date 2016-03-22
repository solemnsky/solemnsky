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

struct PlayerDelta {
  PlayerDelta(); // for unpacking
  PlayerDelta(const class Player &player);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, admin, team);
  }

  optional<std::string> nickname;
  bool admin;
  optional<Team> team; // 0 is spectator, 1 left, 2 right
};

/**
 * A Player is a handle to a player in the arena and his potential game state.
 */
struct Player {
  Player(); // for unpacking
  Player(const PID pid);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(pid, nickname, admin, team);
  }

  PID pid;
  std::string nickname;
  bool admin;
  Team team;

  Plane *plane;

  std::vector<void *> subsystemData;

  void applyDelta(const PlayerDelta &delta);
};

/**
 * The mode of the arena; we cycle through these three modes.
 */
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
class Arena {
 private:
  PID allocPid() const;
  std::string allocNickname(const std::string &requested) const;

  std::vector<Subsystem *> subsystems;

  void initSky(const SkyInitializer &initializer);
  void addPlayer(const Player &player);
  void removePlayer(const Player &player);

 public:
  Arena();

  /**
   * State.
   */
  std::list<Player> players;
  std::string motd; // message of the day

  ArenaMode mode;
  optional<Sky> sky;
  // when in game, sky is instantiated, and we sync Players with Planes

  /**
   * Subsystem.
   */
  void linkSystem(Subsystem *subsystem);

  /**
   * Initializers / deltas.
   */
  void applyInitializer(const ArenaInitializer &initializer);
  optional<ClientEvent> applyDelta(const ArenaDelta &delta);
  ArenaInitializer captureInitializer();

  /**
   * General API. Used by the server.
   */
  Player &connectPlayer(const std::string &requestedNick);
  void disconnectPlayer(const Player &record);
  Player *getPlayer(const PID pid);
};

}
