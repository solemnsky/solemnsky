/**
 * A model of a multiplayer arena, (players + metadata + potential sky) held by
 * clients and servers.
 *
 * A server, during operation, holds an arena; a multiplayer client constructs
 * one from a network-transmitted ArenaInitializer and keeps somewhat in sync
 * with the server-side arena model through deltas.
 */
#ifndef SOLEMNSKY_ARENA_H
#define SOLEMNSKY_ARENA_H

#include "util/types.h"
#include "sky.h"
#include <map>
#include <list>
#include <vector>

namespace sky {

typedef unsigned char Team; // 0 spec, 1 left, 2 right

/**
 * Compact way to change a player to a new state.
 */
struct PlayerDelta {
  PlayerDelta(); // for unpacking
  PlayerDelta(const class Player &player); // set non-optional values

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, admin, team);
  }

  optional<std::string> nickname;
  bool admin; // these values are non-optional
  optional<Team> team; // 0 is spectator, 1 left, 2 right
};

/**
 * A player in the arena.
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

/**
 * The data a client needs when jumping into an arena. Further changes are
 * transmitted through ArenaDelta's.
 */
struct ArenaInitializer: public VerifyStructure {
  ArenaInitializer(); // for unpacking
  ArenaInitializer(
      const std::list<Player> players,
      const std::string &motd,
      const ArenaMode mode,
      const optional<SkyInitializer> skyInitializer);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(players, motd, mode, skyInitializer);
  }

  std::vector<Player> players;
  std::string motd;
  ArenaMode mode;
  optional<SkyInitializer> skyInitializer;

  bool verifyStructure() const override;
};

/**
 * Server-generated modification to the arena.
 */
struct ArenaDelta: public VerifyStructure {
  enum class Type {
    Quit, // a player quits
    Join, // a player joints
    Modify, // a player's data is modified
    Motd, // the motd changes
    Mode // the mode changes
  };

  ArenaDelta();
  ArenaDelta(
      const Type type,
      const optional<PID> &quit = {},
      const optional<Player> &join = {},
      const optional<std::pair<PID, PlayerDelta>> &player = {},
      const optional<std::string> motd = {},
      const optional<ArenaMode> arenaMode = {},
      const optional<SkyInitializer> skyInitializer = {});

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type, quit, join, player, motd, arenaMode, skyInitializer);
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
  static ArenaDelta Modify(const PID, const PlayerDelta &delta);
  static ArenaDelta Motd(const std::string &motd);
  static ArenaDelta Mode(const ArenaMode,
                         const optional<SkyInitializer> &initializer = {});
};

/**
 * Arena.
 */
class Arena {
 public:
  Arena();

  /**
   * State.
   */
  std::list<Player> players;
  std::string motd; // message of the day

  ArenaMode mode;
  optional<Sky> sky;

  /**
   * Initializers / Deltas.
   */
  void applyInitializer(const ArenaInitializer &initializer);
  void applyDelta(const ArenaDelta &delta);
  ArenaInitializer captureInitializer();

  /**
   * General API.
   */
  Player &connectPlayer();
  void disconnectPlayer(const Player &record);
  Player *getPlayer(const PID pid);
};

}

#endif //SOLEMNSKY_ARENA_H
