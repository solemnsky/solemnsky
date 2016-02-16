#include "arena.h"

namespace sky {

/**
 * Player.
 */
Player::Player() { }

Player::Player(const sky::PID pid) :
    pid(pid) { }

bool Player::operator==(const Player &record) {
  return record.pid == pid;
}

/**
 * PlayerDelta.
 */
PlayerDelta::PlayerDelta() { }

PlayerDelta::PlayerDelta(const optional<std::string> &nickname) :
    nickname(nickname) { }

/**
 * ArenaInitializer.
 */

ArenaInitializer::ArenaInitializer() { }


/**
 * ArenaDelta.
 */

ArenaDelta::ArenaDelta() { }

/**
 * Arena.
 */

Arena::Arena() { }

Arena::Arena(const ArenaInitializer &initializer) :
    motd(initializer.motd),
    players(initializer.playerRecords.begin(),
            initializer.playerRecords.end()) { }

/**
 * Shared API.
 */

Player *Arena::getRecord(const PID pid) {
  for (Player &record : players)
    if (record.pid == pid) return &record;
  return nullptr;
}

/**
 * For servers.
 */

Player &Arena::connectPlayer() {
  PID maxPid = 0;
  for (auto &player : players) {
    maxPid = std::max(player.pid, maxPid);
  }
  players.push_back(Player(maxPid + 1));
  return players.back();
}

void Arena::disconnectPlayer(const Player &record) {
  players.remove(record);
}

void Arena::modifyPlayer(const PID pid, PlayerDelta &delta) {

}

ArenaInitializer Arena::captureInitializer() {
  ArenaInitializer initializer;
  initializer.playerRecords =
      std::vector<Player>(players.begin(), players.end());
  initializer.motd = motd;

  return initializer;
}

/**
 * For clients.
 */

void Arena::applyDelta(const ArenaDelta &delta) {
  if (const auto &quit = delta.playerQuit) {
    players.remove_if([&](const Player &record) {
      return record.pid == *quit;
    });
    return;
  }

  if (const auto &join = delta.playerJoin) {
    if (Player *existingRecord = getRecord(join->pid))
      *existingRecord = *join;
    else players.push_back(*join);
    return;
  }

  if (const auto &pdelta = delta.playerDelta) {
    if (Player *record = getRecord(pdelta->first)) {
      if (pdelta->second.nickname)
        record->nickname = *pdelta->second.nickname;
      // ... potentially other things ...
    }
    return;
  }

  if (const auto &newMotd = delta.motdDelta) {
    motd = *newMotd;
  }
}

}
