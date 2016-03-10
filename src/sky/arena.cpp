#include "arena.h"

namespace sky {

/**
 * PlayerDelta.
 */
PlayerDelta::PlayerDelta() { }

PlayerDelta::PlayerDelta(const Player &player) :
    admin(player.admin) { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlayerDelta, TYPE>(RULE, &PlayerDelta::PTR)
PlayerDeltaPack::PlayerDeltaPack() :
    tg::ClassPack<PlayerDelta>(
        member(optional<std::string>, nickname, tg::optStringPack),
        member(bool, admin, tg::boolPack),
        member(optional<Team>, team,
               tg::OptionalPack<Team>(tg::BytePack<Team>()))
    ) { }
#undef member

/**
 * Player.
 */
Player::Player() { }

Player::Player(const sky::PID pid) :
    pid(pid), admin(false) { }

void Player::applyDelta(const PlayerDelta &delta) {
  if (delta.nickname) nickname = *delta.nickname;
  admin = delta.admin;
  if (delta.team) team = *delta.team;
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<Player, TYPE>(RULE, &Player::PTR)
PlayerPack::PlayerPack() :
    tg::ClassPack<Player>(
        member(PID, pid, pidPack),
        member(std::string, nickname, tg::stringPack),
        member(bool, admin, tg::boolPack),
        member(Team, team, tg::BytePack<Team>())
    ) { }
#undef member

/**
 * ArenaInitializer.
 */

ArenaInitializer::ArenaInitializer() { }

ArenaInitializer::ArenaInitializer(
    const std::list<Player> players,
    const std::string &motd,
    const ArenaMode mode,
    const optional<SkyInitializer> skyInitializer) :
    players(players.begin(), players.end()),
    motd(motd),
    mode(mode),
    skyInitializer(skyInitializer) { }

bool ArenaInitializer::verifyStructure() const {
  switch (mode) {
    case ArenaMode::Lobby:
      return true;
    case ArenaMode::Game:
      return verifyFields(skyInitializer);
    case ArenaMode::Scoring:
      return true;
  }
  return false;
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaInitializer, TYPE>(RULE, &ArenaInitializer::PTR)
ArenaInitializerPack::ArenaInitializerPack() :
    tg::ClassPack<ArenaInitializer>(
        tg::MemberRule<ArenaInitializer, std::vector<Player>>(
            tg::VectorPack<Player>(PlayerPack()),
            &ArenaInitializer::players),
        member(std::string, motd, tg::stringPack),
        member(ArenaMode, mode, arenaModePack)
    ) { }
#undef member

/**
 * ArenaDelta.
 */

ArenaDelta::ArenaDelta() { }

ArenaDelta::ArenaDelta(
    const ArenaDelta::Type type,
    const optional<PID> &quit,
    const optional<Player> &join,
    const optional<std::pair<PID, PlayerDelta>> &player,
    const optional<std::string> motd,
    const optional<ArenaMode> arenaMode,
    const optional<SkyInitializer> skyInitializer) :
    type(type),
    quit(quit),
    join(join),
    player(player),
    motd(motd),
    arenaMode(arenaMode),
    skyInitializer(skyInitializer) { }

bool ArenaDelta::verifyStructure() const {
  switch (type) {
    case Type::Quit:
      return verifyFields(quit);
    case Type::Join:
      return verifyFields(join);
    case Type::Modify:
      return verifyFields(player);
    case Type::Motd:
      break;
      return verifyFields(motd);
    case Type::Mode:
      return verifyFields(arenaMode, skyInitializer);
  }
  return false;
}

ArenaDelta ArenaDelta::Quit(const PID pid) {
  return ArenaDelta(ArenaDelta::Type::Quit, pid);
}

ArenaDelta ArenaDelta::Join(const Player &player) {
  return ArenaDelta(ArenaDelta::Type::Join, {}, player);
}

ArenaDelta ArenaDelta::Modify(const PID pid, const PlayerDelta &delta) {
  return ArenaDelta(ArenaDelta::Type::Modify, {}, {},
                    std::pair<PID, PlayerDelta>(pid, delta));
}

ArenaDelta ArenaDelta::Motd(const std::string &motd) {
  return ArenaDelta(ArenaDelta::Type::Motd, {}, {}, {}, motd);
}

ArenaDelta ArenaDelta::Mode(const ArenaMode arenaMode,
                            const optional<SkyInitializer> &initializer) {
  return ArenaDelta(ArenaDelta::Type::Mode, {}, {}, {}, {},
                    arenaMode, initializer);
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ArenaDelta, TYPE>(RULE, &ArenaDelta::PTR)
ArenaDeltaPack::ArenaDeltaPack() :
    tg::ClassPack<ArenaDelta>(
        member(ArenaDelta::Type, type, tg::EnumPack<ArenaDelta::Type>(3)),
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

void Arena::applyInitializer(const ArenaInitializer &initializer) {
  motd = initializer.motd;
  players = std::list<Player>(initializer.players.begin(),
                              initializer.players.end());
  mode = initializer.mode;
  if (initializer.mode == ArenaMode::Game) {
    sky.emplace(*initializer.skyInitializer);
  }
}

void Arena::applyDelta(const ArenaDelta &delta) {
  switch (delta.type) {
    case ArenaDelta::Type::Quit:
      players.remove_if([&](const Player &record) {
        return record.pid == *delta.quit;
      });
    case ArenaDelta::Type::Join: {
      if (Player *existingRecord = getPlayer(delta.join->pid))
        *existingRecord = *delta.join;
      else players.push_back(*delta.join);
    }
    case ArenaDelta::Type::Modify:
      if (Player *player = getPlayer(delta.player->first)) {
        player->applyDelta(delta.player->second);
      }
    case ArenaDelta::Type::Motd:
      motd = *delta.motd;
    case ArenaDelta::Type::Mode: {
      mode = *delta.arenaMode;
      if (mode == ArenaMode::Game) sky.emplace(*delta.skyInitializer);
    }
  }
}

ArenaInitializer Arena::captureInitializer() {
  if (mode == ArenaMode::Game)
    return ArenaInitializer(players, motd, mode, sky->captureInitializer());
  else return ArenaInitializer(players, motd, mode, {});
}

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

Player *Arena::getPlayer(const PID pid) {
  for (Player &record : players)
    if (record.pid == pid) return &record;
  return nullptr;
}
}
