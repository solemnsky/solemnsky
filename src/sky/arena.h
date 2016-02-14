/**
 * A model of a multiplayer arena, (players + metadata + potential sky) held by
 * clients and servers. A server, during operation, holds an arena; a
 * multiplayer client constructs one from a network-transmitted
 * ArenaInitializer and keeps somewhat in sync with the server-side arena
 * model through deltas.
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

  bool operator==(const Player &record); // are PIDs equal?
};

extern const tg::Pack<Player> playerPack;

/**
 * A change in a Player.
 */
struct PlayerDelta {
  PlayerDelta(); // for unpacking
  PlayerDelta(const optional<std::string> &nickname);

  optional<std::string> nickname; // exists if nickname changed

  // ... potentially other things ...
};

extern const tg::Pack<PlayerDelta> playerDeltaPack;

/**
 * The data a client needs when jumping into an arena. Further changes are
 * transmitted through ArenaDelta's.
 */
struct ArenaInitializer {
  ArenaInitializer(); // for unpacking

  std::vector<Player> playerRecords;
  std::string motd;
};

extern const tg::Pack<ArenaInitializer> arenaInitializerPack;

/**
 * A change that occurred in an Arena.
 */
struct ArenaDelta {
  ArenaDelta(); // for unpacking

  // exactly one of these is non-null
  optional<PID> playerQuit;
  optional<Player> playerJoin;
  optional<std::pair<PID, PlayerDelta>> playerDelta;
  optional<std::string> motdDelta;
};

extern const tg::Pack<ArenaDelta> arenaDeltaPack;

/**
 * Arena.
 */
class Arena {
 public:
  Arena();
  Arena(const ArenaInitializer &initializer);

  /**
   * Data.
   */
  std::list<Player> players;
  std::string motd; // the arena MotD

  // ... sky instantiation data ...

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
