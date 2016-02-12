#include "arena.h"

namespace sky {

PlayerRecord::PlayerRecord(const sky::PID pid) :
    pid(pid) { }

bool PlayerRecord::operator==(const PlayerRecord &record) {
  return record.pid == pid;
}

/**
 * Arena.
 */

PlayerRecord &Arena::connectPlayer() {
  PID maxPid = 0;
  for (auto &player : playerRecords) {
    maxPid = std::max(player.pid, maxPid);
  }
  playerRecords.push_back(PlayerRecord(maxPid + 1));
  return playerRecords.back();
}

void Arena::disconnectPlayer(const PlayerRecord &record) {
  playerRecords.remove(record);
}

}
