/**
 * Here we define the the 'Arena', a place where multiple players come together
 * to organise and play games, and related concepts. Each multiplayer server
 * has exactly one arena, which stays online during continuous server
 * operation.
 *
 * We allocate a place in the central game engine for this because many of
 * the concepts here are shared amongst client and server implementations, as
 * well as Sky itself.
*/
#ifndef SOLEMNSKY_ARENA_H
#define SOLEMNSKY_ARENA_H
#include "util/types.h"
#include "sky.h"
#include <map>

namespace sky {

struct Player {
  std::string nickname;
};

class Arena {
  std::map<PID, Player> players;
  std::string motd; // the arena MotD
};

}

#endif //SOLEMNSKY_ARENA_H
