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

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<Player, TYPE>(RULE, &Player::PTR)
const tg::Pack<Player> playerPack =
    tg::ClassPack<Player>(
        member(PID, pid, pidPack),
        member(std::string, nickname, tg::stringPack)
    );
#undef member

/**
 * A change in a Player.
 */
struct PlayerDelta {
  PlayerDelta(); // for unpacking
  PlayerDelta(const optional<std::string> &nickname);

  optional<std::string> nickname; // exists if nickname changed

  // ... potentially other things ...
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlayerDelta, TYPE>(RULE, &PlayerDelta::PTR)
const tg::Pack<PlayerDelta> playerDeltaPack =
    tg::ClassPack<PlayerDelta>(
        member(optional<std::string>, nickname, tg::optStringPack)
    );
#undef member

/**
 * The data a client needs when jumping into an arena. Further changes are
 * transmitted through ArenaDelta's.
 */
struct ArenaInitializer {
  ArenaInitializer(); // for unpacking

  std::vector<Player> playerRecords;
  std::string motd;
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaInitializer, TYPE>(RULE, &ArenaInitializer::PTR)
static const tg::Pack<ArenaInitializer> arenaInitializerPack =
    tg::ClassPack<ArenaInitializer>(
        tg::MemberRule<ArenaInitializer, std::vector<Player>>(
            tg::VectorPack<Player>(playerPack),
            &ArenaInitializer::playerRecords),
        member(std::string, motd, tg::stringPack)
    );
#undef member

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
            // oh boy
            &ArenaDelta::playerDelta
        ),
        member(optional<std::string>, motdDelta, tg::optStringPack)
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
