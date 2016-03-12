#include "arena.h"
#include "util/methods.h"

namespace sky {

/**
 * PlayerDelta.
 */
PlayerDelta::PlayerDelta() { }

PlayerDelta::PlayerDelta(const Player &player) :
    admin(player.admin) { }

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
    case ArenaDelta::Type::Quit: {
      players.remove_if([&](const Player &record) {
        return record.pid == *delta.quit;
      });
      break;
    }
    case ArenaDelta::Type::Join: {
      if (Player *existingRecord = getPlayer(delta.join->pid))
        *existingRecord = *delta.join;
      else players.push_back(*delta.join);
      break;
    }
    case ArenaDelta::Type::Modify: {
      if (Player *player = getPlayer(delta.player->first)) {
        player->applyDelta(delta.player->second);
      }
      break;
    }
    case ArenaDelta::Type::Motd: {
      motd = *delta.motd;
      break;
    }
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
