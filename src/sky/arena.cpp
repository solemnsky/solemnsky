#include "arena.h"
#include "util/methods.h"

namespace sky {

PlayerRecord::PlayerRecord() { }

PlayerRecord::PlayerRecord(const sky::PID pid) :
    pid(pid), connected(false) { }

bool PlayerRecord::operator==(const PlayerRecord &record) {
  return record.pid == pid;
}

/**
 * Arena.
 */

Arena::Arena() { }

/**
 * Shared API.
 */

PlayerRecord *Arena::getRecord(const PID pid) {
  for (PlayerRecord &record : playerRecords)
    if (record.pid == pid) return &record;
  return nullptr;
}

/**
 * For servers.
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

/**
 * For clients.
 */

void Arena::applyConnection(const PlayerRecord &record) {
  if (PlayerRecord *existingRecord = getRecord(record.pid)) {
    *existingRecord = record;
  } else {
    playerRecords.push_back(record);
  }
}

void Arena::applyRecordDelta(const PID pid, const PlayerRecordDelta delta) {
  if (PlayerRecord *record = getRecord(pid)) {
    record->connected = delta.connected;
    if (delta.nickname)
      record->nickname = *delta.nickname;
  }
}

void Arena::applyDisconnection(const PID pid) {
  playerRecords.remove_if([](const PlayerRecord &record) {
    return record.pid == pid;
  });
}

}
