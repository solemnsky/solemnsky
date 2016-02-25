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
PlayerPack::PlayerPack() :
    tg::ClassPack<Player>(
        member(PID, pid, pidPack),
        member(std::string, nickname, tg::stringPack),
        member(bool, admin, tg::boolPack)
    ) { }
#undef member

/**
 * PlayerDelta.
 */
PlayerDelta::PlayerDelta() { }

PlayerDelta::PlayerDelta(const optional<std::string> &nickname) :
    nickname(nickname) { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlayerDelta, TYPE>(RULE, &PlayerDelta::PTR)
PlayerDeltaPack::PlayerDeltaPack() :
    tg::ClassPack<PlayerDelta>(
        member(optional<std::string>, nickname, tg::optStringPack),
        member(optional<bool>, admin, tg::OptionalPack<bool>(tg::boolPack))
    ) { }
#undef member

/**
 * ArenaInitializer.
 */

ArenaInitializer::ArenaInitializer() { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaInitializer, TYPE>(RULE, &ArenaInitializer::PTR)
ArenaInitializerPack::ArenaInitializerPack() :
    tg::ClassPack<ArenaInitializer>(
        tg::MemberRule<ArenaInitializer, std::vector<Player>>(
            tg::VectorPack<Player>(PlayerPack()),
            &ArenaInitializer::playerRecords),
        member(std::string, motd, tg::stringPack),
        member(ArenaMode, mode, arenaModePack),
        member(std::string, nextMap, tg::stringPack)
    ) { }
#undef member

/**
 * ArenaDelta.
 */

ArenaDelta::ArenaDelta() { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaDelta, TYPE>(RULE, &ArenaDelta::PTR)
ArenaDeltaPack::ArenaDeltaPack() :
    tg::ClassPack<ArenaDelta>(
        member(optional<PID>, quit, tg::OptionalPack<PID>(pidPack)),
        member(optional<Player>, join,
               tg::OptionalPack<Player>(PlayerPack())),
        tg::MemberRule<ArenaDelta, optional<std::pair<PID, PlayerDelta>>>(
            tg::OptionalPack<std::pair<PID, PlayerDelta>>(
                tg::PairPack<PID, PlayerDelta>(pidPack, PlayerDeltaPack())),
            &ArenaDelta::player
        ),
        member(optional<std::string>, motd, tg::optStringPack),
        member(optional<ArenaMode>, arenaMode,
               tg::OptionalPack<ArenaMode>(arenaModePack)),
        member(optional<SkyInitializer>, skyInitializer,
               tg::OptionalPack<SkyInitializer>(SkyInitializerPack()))
    ) { }
#undef member

/**
 * Arena.
 */

Arena::Arena() { }

/**
 * Initializers / Deltas.
 */

bool Arena::applyInitializer(const ArenaInitializer &initializer) {
  motd = initializer.motd;
  players = std::list(initializer.playerRecords.begin(), initializer
      .playerRecords.end());
  mode = initializer.mode;
  if (initializer.mode == ArenaMode::Game) {
    if (!all(initializer.skyInitializer)) return false;
    sky.emplace(sky::Sky(*initializer.skyInitializer));
  }

  return true;
}

bool Arena::applyDelta(const ArenaDelta &delta) {
  if (delta.quit) {
    players.remove_if([&](const Player &record) {
      return record.pid == *delta.quit;
    });
    return true;
  }

  if (delta.join) {
    if (Player *existingRecord = getRecord(delta.join->pid))
      *existingRecord = *delta.join;
    else players.push_back(*delta.join);
    return true;
  }

  if (delta.player) {
    if (Player *record = getRecord(delta.player->first)) {
      if (delta.player->second.nickname)
        record->nickname = *delta.player->second.nickname;
      // ... potentially other things ...
    }
    return true;
  }

  if (delta.motd) {
    motd = *delta.motd;
    return true;
  }

  if (delta.arenaMode) {
    ArenaMode newMode = *delta.arenaMode;
    switch (newMode) {
      case ArenaMode::Lobby: {
        mode = newMode;
        return true;
      }
      case ArenaMode::Game: {
        if (!delta.skyInitializer) return false;
        sky.emplace(sky::Sky(*delta.skyInitializer));
        return true;
      }
      case ArenaMode::Scoring: {
        mode = newMode;
        // TODO: scoring information or something
        return true;
      }
    }
  }

  return false; // no delta was provided, this counts as a malformation
}


/**
 * API.
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

Player *Arena::getRecord(const PID pid) {
  for (Player &record : players)
    if (record.pid == pid) return &record;
  return nullptr;
}

void Arena::enterLobby() {
  mode = ArenaMode::Lobby;
}

void Arena::enterGame(const SkyInitializer &initializer) {
  mode = ArenaMode::Game;
  sky.emplace(sky::Sky(initializer));
}

void Arena::enterScoring() {
  mode = ArenaMode::Scoring;
}

ArenaInitializer Arena::captureInitializer() {
  ArenaInitializer initializer;
  initializer.playerRecords =
      std::vector<Player>(players.begin(), players.end());
  initializer.motd = motd;

  return initializer;
}

}
