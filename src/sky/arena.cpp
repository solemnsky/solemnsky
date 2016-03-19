#include "arena.h"
#include <algorithm>
#include "util/methods.h"
#include "event.h"

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
    pid(pid), admin(false), team(0) { }

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


PID Arena::allocPid() const {
  if (players.empty()) return 0;
  PID maxPid = 0;
  for (const auto &player : players) maxPid = std::max(player.pid, maxPid);
  return maxPid + 1;
}

std::string Arena::allocNickname(const std::string &requested) const {
  std::stringstream readStream;
  int nickNumber;
  const size_t rsize = requested.size();

  const size_t reqSize = rsize;
  std::vector<int> usedNumbers;
  for (const auto &player : players) {
    const std::string &name = player.nickname;
    if (name.size() < rsize) continue;
    if (name.substr(0, rsize) != requested) continue;

    if (name == requested) {
      usedNumbers.push_back(0);
      continue;
    }

    readStream.str(name.substr(rsize));
    if (readStream.get() != '(') continue;
    readStream >> nickNumber;
    if (!readStream.good()) continue;
    if (readStream.get() != ')') continue;
    if (!readStream.eof()) continue;

    usedNumbers.push_back(nickNumber);
  }

  if (usedNumbers.empty()) return requested;

  int maxNum = 0;
  for (const int val : usedNumbers) maxNum = std::max(val, maxNum);
  return requested + "(" + std::to_string(maxNum + 1) + ")";
}

void Arena::applyInitializer(const ArenaInitializer &initializer) {
  motd = initializer.motd;
  players = std::list<Player>(initializer.players.begin(),
                              initializer.players.end());
  mode = initializer.mode;
  if (initializer.mode == ArenaMode::Game) {
    sky.emplace(*initializer.skyInitializer);
  }
}

optional<ClientEvent> Arena::applyDelta(const ArenaDelta &delta) {
  switch (delta.type) {
    case ArenaDelta::Type::Quit: {
      optional<std::string> playerQuit;
      players.remove_if([&](const Player &record) {
        if (record.pid == *delta.quit) {
          playerQuit = record.nickname;
          return true;
        };
        return false;
      });
      if (playerQuit) {
        return {ClientEvent::Quit(*playerQuit)};
      }
      break;
    }
    case ArenaDelta::Type::Join: {
      if (Player *existingRecord = getPlayer(delta.join->pid))
        *existingRecord = *delta.join;
      else players.push_back(*delta.join);
      return {ClientEvent::Join(delta.join->nickname)};
    }
    case ArenaDelta::Type::Modify: {
      if (Player *player = getPlayer(delta.player->first)) {
        Team oldTeam = player->team;
        std::string oldName = player->nickname;
        player->applyDelta(delta.player->second);
        if (player->nickname != oldName)
          return {ClientEvent::NickChange(oldName, player->nickname)};
        if (player->team != oldTeam)
          return {ClientEvent::TeamChange(player->nickname, oldTeam,
                                          player->team)};
      }
      break;
    }
    case ArenaDelta::Type::Motd: {
      motd = *delta.motd;
      break;
    }
    case ArenaDelta::Type::Mode: {
      mode = *delta.arenaMode;
      if (mode == ArenaMode::Lobby) return ClientEvent::LobbyStart();
      if (mode == ArenaMode::Scoring) return ClientEvent::ScoringStart();
      if (mode == ArenaMode::Game) {
        sky.emplace(*delta.skyInitializer);
        return ClientEvent::GameStart(delta.skyInitializer->mapName);
      }
      break;
    }
  }
  return {};
}

ArenaInitializer Arena::captureInitializer() {
  if (mode == ArenaMode::Game)
    return ArenaInitializer(players, motd, mode, sky->captureInitializer());
  else return ArenaInitializer(players, motd, mode, {});
}

Player &Arena::connectPlayer(const std::string &requestedNick) {
  PID pid = allocPid();
  std::string nickname = allocNickname(requestedNick);
  players.emplace_back(Player(pid));
  Player &newPlayer = players.back();
  newPlayer.nickname = nickname;
  return newPlayer;
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
