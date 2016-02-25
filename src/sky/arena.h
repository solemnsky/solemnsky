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

struct PlayerPack: public tg::ClassPack<Player> {
  PlayerPack();
};

/**
 * A change in a Player.
 */
struct PlayerDelta {
  PlayerDelta(); // for unpacking
  PlayerDelta(const optional<std::string> &nickname);

  optional<std::string> nickname; // exists if nickname changed
  optional<bool> admin; // exists if admin state changed
};

struct PlayerDeltaPack: public tg::ClassPack<PlayerDelta> {
  PlayerDeltaPack();
};

/**
 * ArenaMode: the mode of the arena.
 */
enum class ArenaMode {
  Lobby, // lobby, to make teams
  Game, // playing game
  Scoring // viewing game results
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
  std::string nextMap;
};

struct ArenaInitializerPack: public tg::ClassPack<ArenaInitializer> {
  ArenaInitializerPack();
};

/**
 * A change that occurred in an Arena.
 */
struct ArenaDelta {
  ArenaDelta();

  // exactly one of these blocks has instantiated members
  optional<PID> quit;

  optional<Player> join;

  optional<std::pair<PID, PlayerDelta>> player;

  optional<std::string> motd;

  optional<ArenaMode> arenaMode;
  optional<SkyInitializer> skyInitializer;
};

struct ArenaDeltaPack: public tg::ClassPack<ArenaDelta> {
  ArenaDeltaPack();
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
  bool applyInitializer(const ArenaInitializer &initializer);
  bool applyDelta(const ArenaDelta &delta);

  /**
   * General API.
   */
  Player &connectPlayer();
  void disconnectPlayer(const Player &record);
  void modifyPlayer(const PID pid, PlayerDelta &delta);
  Player *getRecord(const PID pid);

  void enterLobby();
  void enterGame(const SkyInitializer &initializer);
  void enterScoring();

  ArenaInitializer captureInitializer();
};

}

#endif //SOLEMNSKY_ARENA_H
