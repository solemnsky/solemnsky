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
#include "telegraph/pack.h"
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

  optional<std::string> nickname;
  bool admin; // these values are non-optional
  optional<Team> team; // 0 is spectator, 1 left, 2 right
};

static const struct PlayerDeltaPack: public tg::ClassPack<PlayerDelta> {
  PlayerDeltaPack();
} playerDeltaPack;

/**
 * A player in the arena.
 */
struct Player {
  Player(); // for unpacking
  Player(const PID pid);

  PID pid;
  std::string nickname;
  bool admin;
  Team team;

  void applyDelta(const PlayerDelta &delta);
};

static const struct PlayerPack: public tg::ClassPack<Player> {
  PlayerPack();
} playerPack;

/**
 * The mode of the arena; we cycle through these three modes.
 */
enum class ArenaMode {
  Lobby, // lobby, to make teams
  Game, // playing tutorial
  Scoring // viewing tutorial results
};

static const tg::Pack<ArenaMode> arenaModePack = tg::EnumPack<ArenaMode>(2);

/**
 * The data a client needs when jumping into an arena. Further changes are
 * transmitted through ArenaDelta's.
 */
struct ArenaInitializer {
  ArenaInitializer(); // for unpacking
  ArenaInitializer(
      const std::list<Player> players,
      const std::string &motd,
      const ArenaMode mode,
      const optional<SkyInitializer> skyInitializer);

  std::vector<Player> players;
  std::string motd;
  ArenaMode mode;
  optional<SkyInitializer> skyInitializer;
};

static const struct ArenaInitializerPack:
    public tg::ClassPack<ArenaInitializer> {
  ArenaInitializerPack();
} arenaInitializerPack;

/**
 * Server-generated modification to the arena.
 */
struct ArenaDelta {
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

  Type type;
  optional<PID> quit;
  optional<Player> join;
  optional<std::pair<PID, PlayerDelta>> player;
  optional<std::string> motd;
  optional<ArenaMode> arenaMode;
  optional<SkyInitializer> skyInitializer;

  static ArenaDelta Quit(const PID pid);
  static ArenaDelta Join(const Player &player);
  static ArenaDelta Modify(const PID, const PlayerDelta &delta);
  static ArenaDelta Motd(const std::string &motd);
  static ArenaDelta Mode(const ArenaMode,
                         const optional<SkyInitializer> &initializer = {});
};

static const struct ArenaDeltaPack: public tg::ClassPack<ArenaDelta> {
  ArenaDeltaPack();
} arenaDeltaPack;

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
  bool applyInitializer(const ArenaInitializer &initializer);
  bool applyDelta(const ArenaDelta &delta);
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
