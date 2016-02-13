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

struct PlayerRecordDelta {
  bool connected; // sets connection state
  optional<std::string> nickname; // exists if nickname changed
  // ... potentially other things ...
};

class Arena {
 public:
  Arena();

  std::list<PlayerRecord> playerRecords;
  std::string motd; // the arena MotD

  // ... sky instantiation data ...

  /**
   * Shared API.
   */
  PlayerRecord *getRecord(const PID pid);

  /**
   * For servers.
   */
  PlayerRecord &connectPlayer();
  void disconnectPlayer(const PlayerRecord &record);

  /**
   * For clients.
   */
  void applyConnection(const PlayerRecord &record);
  void applyRecordDelta(const PID pid, const PlayerRecordDelta);
  void applyDisconnection(const PID pid);
};

}

#endif //SOLEMNSKY_ARENA_H
