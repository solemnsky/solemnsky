/**
 * Shared model of a multiplayer game arena.
 */
#pragma once
#include "util/types.h"
#include "sky.h"
#include <map>
#include <list>
#include <vector>

namespace sky {

typedef unsigned char Team; // 0 spec, 1 red, 2 blue

/**
 * Delta in a Player.
 */
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
 * A player in the arena, with static data.
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
 * Initialize a new remote Arena.
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
 * Delta in some arena state,
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
 * A model of a game arena, with utilities to help syncing it over a network.
 * Used by both server and client.
 */
class Arena {
 private:
  PID allocPid() const;
  std::string allocNickname(const std::string &requested) const;
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
   * Initializers / deltas.
   */
  void applyInitializer(const ArenaInitializer &initializer);
  optional<struct ClientEvent> applyDelta(const ArenaDelta &delta);
  ArenaInitializer captureInitializer();

  /**
   * General API. Used by the server.
   */
  Player &connectPlayer(const std::string &requestedNick);
  void disconnectPlayer(const Player &record);
  Player *getPlayer(const PID pid);
};

}
