#include "arena.h"
#include "util/methods.h"

namespace sky {

PlayerRecord::PlayerRecord() { }

PlayerRecord::PlayerRecord(const sky::PID pid) :
    pid(pid) { }

bool PlayerRecord::operator==(const PlayerRecord &record) {
  return record.pid == pid;
}

/**
 * Arena.
 */

Arena::Arena() { }

PlayerRecord &Arena::connectPlayer() {
  PID maxPid = 0;
  for (auto &player : playerRecords) {
    maxPid = std::max(player.pid, maxPid);
  }
  playerRecords.push_back(PlayerRecord(maxPid + 1));
  return playerRecords.back();
}

PlayerRecord *Arena::getPlayer(const PID pid) {
  for (PlayerRecord &player : playerRecords)
    if (player.pid == pid) return &player;
  return nullptr;
}

void Arena::disconnectPlayer(const PlayerRecord &record) {
  playerRecords.remove(record);
}

}
