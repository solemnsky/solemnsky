/**
 * A model of a multiplayer arena, held by clients and servers. Persists
 * throughout server function.
 */
#ifndef SOLEMNSKY_ARENA_H
#define SOLEMNSKY_ARENA_H
#include "util/types.h"
#include "sky.h"
#include <map>
#include <list>

namespace sky {

struct PlayerRecord {
  bool connected;

  PID pid;
  std::string nickname;

  PlayerRecord(); // for packing
  PlayerRecord(const PID pid);

  bool operator==(const PlayerRecord &record); // are pids equal
};

class Arena {
 public:
  Arena();

  std::list<PlayerRecord> playerRecords;
  std::string motd; // the arena MotD

  // ... sky instantiation data ...

  PlayerRecord &connectPlayer();
  PlayerRecord * getPlayer(const PID pid);
  void disconnectPlayer(const PlayerRecord &record);
};

}

#endif //SOLEMNSKY_ARENA_H
