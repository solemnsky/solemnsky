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

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<Player, TYPE>(RULE, &Player::PTR)
const tg::Pack<Player> playerPack =
    tg::ClassPack<Player>(
        member(PID, pid, pidPack),
        member(std::string, nickname, tg::stringPack),
        member(bool, admin, tg::boolPack)
    );
#undef member

/**
 * PlayerDelta.
 */
PlayerDelta::PlayerDelta() { }

PlayerDelta::PlayerDelta(const optional<std::string> &nickname) :
    nickname(nickname) { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlayerDelta, TYPE>(RULE, &PlayerDelta::PTR)
const tg::Pack<PlayerDelta> playerDeltaPack =
    tg::ClassPack<PlayerDelta>(
        member(optional<std::string>, nickname, tg::optStringPack),
        member(optional<bool>, admin, tg::OptionalPack<bool>(tg::boolPack))
    );
#undef member

/**
 * ArenaInitializer.
 */

ArenaInitializer::ArenaInitializer() { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaInitializer, TYPE>(RULE, &ArenaInitializer::PTR)
const tg::Pack<ArenaInitializer> arenaInitializerPack =
    tg::ClassPack<ArenaInitializer>(
        tg::MemberRule<ArenaInitializer, std::vector<Player>>(
            tg::VectorPack<Player>(playerPack),
            &ArenaInitializer::playerRecords),
        member(std::string, motd, tg::stringPack),
        member(ArenaMode, mode, arenaModePack)
    );
#undef member

/**
 * ArenaDelta.
 */

ArenaDelta::ArenaDelta() { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaDelta, TYPE>(RULE, &ArenaDelta::PTR)
const tg::Pack<ArenaDelta> arenaDeltaPack =
    tg::ClassPack<ArenaDelta>(
        member(optional<PID>, playerQuit, tg::OptionalPack<PID>(pidPack)),
        member(optional<Player>, playerJoin,
               tg::OptionalPack<Player>(playerPack)),
        tg::MemberRule<ArenaDelta, optional<std::pair<PID, PlayerDelta>>>(
            tg::OptionalPack<std::pair<PID, PlayerDelta>>(
                tg::PairPack<PID, PlayerDelta>(pidPack, playerDeltaPack)),
            &ArenaDelta::playerDelta
        ),
        member(optional<std::string>, motdDelta, tg::optStringPack),
        member(optional<ArenaMode>, arenaMode,
               tg::OptionalPack<ArenaMode>(arenaModePack)),
        member(optional<SkyInitializer>, skyInitializer,
               tg::OptionalPack<SkyInitializer>(skyInitializerPack))
    );
#undef member

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
  players.remove_if([&](const Player &p) {
    return p.pid == record.pid;
  });
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
