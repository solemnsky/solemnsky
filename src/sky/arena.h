/**
 * A model of a multiplayer arena, (players + metadata + potential sky) held by
 * clients and servers, along with serializable structures to communicate them
 * over the network.
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

/**
 * A player in the arena.
 */
struct Player {
  Player(); // for unpacking
  Player(const PID pid);

  PID pid;
  std::string nickname;
  bool admin;

  bool operator==(const Player &record); // are PIDs equal?
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<Player, TYPE>(RULE, &Player::PTR)
static const tg::Pack<Player> playerPack =
    tg::ClassPack<Player>(
        member(PID, pid, pidPack),
        member(std::string, nickname, tg::stringPack),
        member(bool, admin, tg::boolPack)
    );
#undef member

/**
 * A change in a Player.
 */
struct PlayerDelta {
  PlayerDelta(); // for unpacking
  PlayerDelta(const optional<std::string> &nickname);

  optional<std::string> nickname; // exists if nickname changed
  optional<bool> admin; // exists if admin state changed
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlayerDelta, TYPE>(RULE, &PlayerDelta::PTR)
static const tg::Pack<PlayerDelta> playerDeltaPack =
    tg::ClassPack<PlayerDelta>(
        member(optional<std::string>, nickname, tg::optStringPack),
        member(optional<bool>, admin, tg::OptionalPack<bool>(tg::boolPack))
    );
#undef member

/**
 * ArenaMode: the mode of the arena.
 */
enum class ArenaMode {
  Lobby, // lobby, to make teams
  Game, // playing game
  Score // viewing game results
};

static const tg::Pack<ArenaMode> arenaModePack =
    tg::EnumPack<ArenaMode>(2);

/**
 * The data a client needs when jumping into an arena. Further changes are
 * transmitted through ArenaDelta's.
 */
struct ArenaInitializer {
  ArenaInitializer(); // for unpacking

  /**
   * Initialize persistent data.
   */
  std::vector<Player> playerRecords;
  std::string motd;

  /**
   * Initialize mode.
   */
  ArenaMode mode;
  optional<SkyInitializer> skyInitializer;
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaInitializer, TYPE>(RULE, &ArenaInitializer::PTR)
static const tg::Pack<ArenaInitializer> arenaInitializerPack =
    tg::ClassPack<ArenaInitializer>(
        tg::MemberRule<ArenaInitializer, std::vector<Player>>(
            tg::VectorPack<Player>(playerPack),
            &ArenaInitializer::playerRecords),
        member(std::string, motd, tg::stringPack),
        member(ArenaMode, mode, arenaModePack)
    );
#undef member

/**
 * A change that occurred in an Arena.
 */
struct ArenaDelta {
  ArenaDelta(); // for unpacking

  // delta in the players list: at most one of these exists
  optional<PID> playerQuit;
  optional<Player> playerJoin;

  // deltas in the persistent state
  optional<std::pair<PID, PlayerDelta>> playerDelta;
  optional<std::string> motdDelta;

  // mode delta, with potentially necessary mode initialization
  optional<ArenaMode> arenaMode;
  optional<SkyInitializer> skyInitializer; // if the game started
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaDelta, TYPE>(RULE, &ArenaDelta::PTR)
static const tg::Pack<ArenaDelta> arenaDeltaPack =
    tg::ClassPack<ArenaDelta>(
        member(optional<PID>, playerQuit, tg::OptionalPack<PID>(pidPack)),
        member(optional<Player>, playerJoin,
               tg::OptionalPack<Player>(playerPack)),
        tg::MemberRule<ArenaDelta, optional<std::pair<PID, PlayerDelta>>>(
            tg::OptionalPack<std::pair<PID, PlayerDelta>>(
                tg::PairPack<PID, PlayerDelta>(pidPack, playerDeltaPack)),
            &ArenaDelta::playerDelta
        ),
        member(optional<std::string>, motdDelta, tg::optStringPack),
        member(optional<ArenaMode>, arenaMode,
               tg::OptionalPack<ArenaMode>(arenaModePack)),
        member(optional<SkyInitializer>, skyInitializer,
               tg::OptionalPack<SkyInitializer>(skyInitializerPack))
    );
#undef member

/**
 * Arena.
 */
class Arena {
 public:
  Arena();
  Arena(const ArenaInitializer &initializer);

  /**
   * Persistent state.
   */
  std::list<Player> players;
  std::string motd; // the arena MotD

  /**
   * Modal state.
   */
  enum class Mode {
    Lobby, Game, Score
  } mode;

  /**
   * Game-specific state.
   */
  std::string nextMap;
  optional<Sky> sky;

  /**
   * Shared API.
   */
  Player *getRecord(const PID pid);
  void applyDelta(const ArenaDelta &);

  /**
   * For servers.
   */
  Player &connectPlayer();
  void disconnectPlayer(const Player &record);
  void modifyPlayer(const PID pid, PlayerDelta &delta);

  ArenaInitializer captureInitializer();
};

}

#endif //SOLEMNSKY_ARENA_H
