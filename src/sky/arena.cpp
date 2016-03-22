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

bool ArenaInitializer::verifyStructure() const {
  if (mode == ArenaMode::Game) return verifyFields(skyInitializer);
  return true;
}

/**
 * ArenaDelta.
 */

ArenaDelta::ArenaDelta() { }

ArenaDelta::ArenaDelta(const ArenaDelta::Type type) : type(type) { }

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
      if (!verifyFields(arenaMode)) return false;
      if (*arenaMode == ArenaMode::Game)
        return verifyFields(skyInitializer);
      else return true;
  }
  return false;
}

ArenaDelta ArenaDelta::Quit(const PID pid) {
  ArenaDelta delta(Type::Quit);
  delta.quit = pid;
  return delta;
}

ArenaDelta ArenaDelta::Join(const Player &player) {
  ArenaDelta delta(Type::Join);
  delta.join = player;
  return delta;
}

ArenaDelta ArenaDelta::Modify(const PID pid, const PlayerDelta &pDelta) {
  ArenaDelta delta(Type::Modify);
  delta.player = std::pair<PID, PlayerDelta>(pid, pDelta);
  return delta;
}

ArenaDelta ArenaDelta::Motd(const std::string &motd) {
  ArenaDelta delta(Type::Motd);
  delta.motd = motd;
  return delta;
}

ArenaDelta ArenaDelta::Mode(const ArenaMode arenaMode,
                            const optional<SkyInitializer> &skyInitializer) {
  ArenaDelta delta(Type::Mode);
  delta.arenaMode = arenaMode;
  delta.skyInitializer = skyInitializer;
  return delta;
}

/**
 * Subsystem.
 */

Subsystem::Subsystem(const PID id, Arena *arena) :
    id(id), arena(arena) { }

/**
 * Arena.
 */

PID Arena::allocPid() const {
  std::vector<int> usedPids;
  for (const auto &player : players) usedPids.push_back(player.pid);
  return (PID) smallestUnused(usedPids);
}

std::string Arena::allocNickname(const std::string &requested) const {
  std::stringstream readStream;
  int nickNumber;
  const size_t rsize = requested.size();

  const size_t reqSize = rsize;
  std::vector<int> usedNumbers;

  for (const auto &player : players) {
    const std::string &name = player.nickname;
    appLog("Checking name: " + name);
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
    readStream.get();
    if (!readStream.eof()) continue;

    usedNumbers.push_back(nickNumber);
  }

  if (usedNumbers.empty()) return requested;

  return requested + "(" + std::to_string(smallestUnused(usedNumbers)) + ")";
}

void Arena::initSky(const SkyInitializer &initializer) {
  sky.emplace(initializer);
  for (const auto &player : players) {
    player.plane = sky->getPlane(player.pid);
  }
}

void Arena::join(const Player &player) {
  if (sky) sky->onJoin(player);
}

void Arena::quit(const Player &player) {

}

Arena::Arena() { }

void Arena::linkSystem(Subsystem *subsystem) {
  subsystems.push_back(subsystem);
}

void Arena::applyInitializer(const ArenaInitializer &initializer) {
  motd = initializer.motd;
  players = std::list<Player>(initializer.players.begin(),
                              initializer.players.end());
  mode = initializer.mode;
  if (mode == ArenaMode::Game) {
    initSky(*initializer.skyInitializer);
  }
}

void Arena::applyDelta(const ArenaDelta &delta) {
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
      if (mode == ArenaMode::Game) sky.emplace(*delta.skyInitializer);
      else sky.reset();
      break;
    }
  }
  return {};
}

ArenaInitializer Arena::captureInitializer() {
  ArenaInitializer initializer;
  initializer.players =
      std::vector<Player>(players.begin(), players.end());
  initializer.motd = motd;
  initializer.mode = mode;
  if (sky)
    initializer.skyInitializer = sky->captureInitializer();
  return initializer;
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
