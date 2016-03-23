#include "arena.h"
#include <algorithm>
#include "util/methods.h"
#include "event.h"

namespace sky {

/**
 * Player.
 */

PlayerInitializer::PlayerInitializer(const std::string &nickname) :
    nickname(nickname), admin(false), team(0) { }

Player::Player(const PlayerInitializer &initializer) :
    Networked(initializer),
    nickname(initializer.nickname),
    admin(initializer.admin),
    team(initializer.team) { }

void Player::applyDelta(const PlayerDelta &delta) {
  if (delta.nickname) nickname = *delta.nickname;
  admin = delta.admin;
  if (delta.team) team = *delta.team;
}

PlayerInitializer Player::captureInitializer() const {
  PlayerInitializer initializer;
  initializer.nickname = nickname;
  initializer.admin = admin;
  initializer.team = team;
  return initializer;
}

PlayerDelta Player::zeroDelta() const {
  PlayerDelta delta;
  delta.admin = admin;
  return delta;
}

/**
 * Arena.
 */

ArenaDelta::ArenaDelta(const ArenaDelta::Type type) : type(type) { }

bool ArenaDelta::verifyStructure() const {
  switch (type) {
    case Type::Quit:
      return verifyFields(quit);
    case Type::Join:
      return verifyFields(join);
    case Type::Delta:
      return verifyFields(delta);
    case Type::Motd:
      return verifyFields(motd);
    case Type::Mode:
      return verifyFields(mode);
  }
  return false; // enum out of bounds
}

ArenaDelta ArenaDelta::Quit(const PID pid) {
  ArenaDelta delta(Type::Quit);
  delta.quit = pid;
  return delta;
}

ArenaDelta ArenaDelta::Join(const PID pid, const Player &player) {
  ArenaDelta delta(Type::Join);
  delta.join = std::pair<PID, Player>(pid, player);
  return delta;
}

ArenaDelta ArenaDelta::Delta(const PID pid, const PlayerDelta &delta) {
  ArenaDelta adelta(Type::Delta);
  adelta.delta = std::pair<PID, PlayerDelta>(pid, delta);
  return adelta;
}

ArenaDelta ArenaDelta::Motd(const std::string &motd) {
  ArenaDelta delta(Type::Motd);
  delta.motd = motd;
  return delta;
}

ArenaDelta ArenaDelta::Mode(const ArenaMode arenaMode) {
  ArenaDelta delta(Type::Mode);
  delta.mode = arenaMode;
  return delta;
}

/**
 * Subsystem.
 */

Subsystem::Subsystem(Arena *arena) :
    id(PID(arena->subsystems.size())) {
  arena->subsystems.push_back(this);
  for (auto &player : arena->players) {
    player.data.push_back(nullptr);
    initialize(player);
  }
}

void Subsystem::initialize(Player &player) { }

void Subsystem::tick(const float delta) { }

void Subsystem::join(Player &player) { }

void Subsystem::quit(Player &player) { }

/**
 * Arena.
 */

ArenaInitializer::ArenaInitializer(const std::string &name) :
    name(name), mode(ArenaMode::Lobby) { }

PID Arena::allocPid() const {
  std::vector<int> usedPids;
  for (const auto &player : players) usedPids.push_back(player.first);
  return (PID) smallestUnused(usedPids);
}

std::string Arena::allocNickname(const std::string &requested) const {
  std::stringstream readStream;
  int nickNumber;
  const size_t rsize = requested.size();

  const size_t reqSize = rsize;
  std::vector<int> usedNumbers;

  for (const auto &player : players) {
    const std::string &name = player.second.nickname;
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

Arena::Arena(const ArenaInitializer &initializer) :
    Networked(initializer),
    name(initializer.name),
    motd(initializer.motd),
    mode(initializer.mode) {
  for (auto const &player : initializer.players) {
    players.emplace(player.first, player.second);
  }
}

void Arena::applyDelta(const ArenaDelta &delta) {
  switch (delta.type) {
    case ArenaDelta::Type::Quit: {
      optional<std::string> playerQuit;
      if (auto player = players.find(*delta.quit) != players.end()) {
        players.erase(player);
        return {ClientEvent::Quit(player->second.nickname)};
      } else return {};
      break;
    }
    case ArenaDelta::Type::Join: {
      if (Player *existingRecord = getPlayer(delta.join->pid))
        *existingRecord = *delta.join;
      else players.push_back(*delta.join);
      return {ClientEvent::Join(delta.join->nickname)};
    }
    case ArenaDelta::Type::Delta: {
      if (Player *player = getPlayer(delta.delta->first)) {
        Team oldTeam = player->team;
        std::string oldName = player->nickname;
        player->applyDelta(delta.delta->second);
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
      mode = *delta.mode;
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

Player &Arena::joinPlayer(const std::string &requestedNick) {
  PID pid = allocPid();
  std::string nickname = allocNickname(requestedNick);
  players.emplace_back(Player(pid));
  Player &newPlayer = players.back();
  newPlayer.nickname = nickname;
  return newPlayer;
}

void Arena::quitPlayer(const Player &player) {
  players.remove_if([&](const Player &p) {
    return p.pid == player.pid;
  });
}

Player *Arena::getPlayer(const PID pid) {
  for (Player &record : players)
    if (record.pid == pid) return &record;
  return nullptr;
}
}
