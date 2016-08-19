/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/algorithm/string.hpp>
#include "arena.hpp"
#include "event.hpp"

namespace sky {

/**
 * Arena.
 */

ArenaDelta::ArenaDelta(const ArenaDelta::Type type) : type(type) { }

bool ArenaDelta::verifyStructure() const {
  switch (type) {
    case Type::Quit:
      return verifyRequiredOptionals(quit);
    case Type::Join:
      return verifyRequiredOptionals(join);
    case Type::Delta:
      return verifyRequiredOptionals(playerDeltas);
    case Type::ResetEnvLoad:
      return true;
    case Type::Motd:
      return verifyRequiredOptionals(motd);
    case Type::Mode:
      return verifyRequiredOptionals(mode);
    case Type::EnvChange:
      return verifyRequiredOptionals(environment);
    case Type::TeamCount:
      return verifyRequiredOptionals(teamCount);
  }
  return false; // enum out of bounds
}

ArenaDelta ArenaDelta::Quit(const PID pid) {
  ArenaDelta delta(Type::Quit);
  delta.quit = pid;
  return delta;
}

ArenaDelta ArenaDelta::Join(const PlayerInitializer &initializer) {
  ArenaDelta delta(Type::Join);
  delta.join = initializer;
  return delta;
}

ArenaDelta ArenaDelta::Delta(const PID pid, const PlayerDelta &playerDelta) {
  ArenaDelta delta(Type::Delta);
  delta.playerDeltas.emplace();
  delta.playerDeltas->emplace(pid, playerDelta);
  return delta;
}

ArenaDelta ArenaDelta::Delta(const std::map<PID, PlayerDelta> &playerDeltas) {
  ArenaDelta delta(Type::Delta);
  delta.playerDeltas = playerDeltas;
  return delta;
}

ArenaDelta ArenaDelta::ResetEnvLoad() {
  return ArenaDelta(Type::ResetEnvLoad);
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

ArenaDelta ArenaDelta::EnvChange(const EnvironmentURL &map) {
  ArenaDelta delta(Type::EnvChange);
  delta.environment = map;
  return delta;
}

ArenaDelta ArenaDelta::TeamCount(const int &teamCount) {
  ArenaDelta delta(Type::TeamCount);
  delta.teamCount = teamCount;
  return delta;
}

/**
 * SubsystemListener.
 */

void SubsystemListener::registerPlayer(Player &) { }

void SubsystemListener::unregisterPlayer(Player &) { }

void SubsystemListener::onPoll() { }

void SubsystemListener::onTick(const TimeDiff) { }

void SubsystemListener::onDebugRefresh() { }

void SubsystemListener::onJoin(Player &) { }

void SubsystemListener::onQuit(Player &) { }

void SubsystemListener::onMode(const ArenaMode) { }

void SubsystemListener::onMapChange() { }

void SubsystemListener::onDelta(Player &, const PlayerDelta &) { }

void SubsystemListener::onSpawn(Player &) { }

void SubsystemListener::onKill(Player &) { }

void SubsystemListener::onStartGame() { }

void SubsystemListener::onEndGame() { }

/**
 * SubsystemCaller.
 */

SubsystemCaller::SubsystemCaller(Arena &arena) :
    arena(arena) { }

void SubsystemCaller::doSpawn(Player &player) {
  for (const auto &pair : arena.subsystems)
    pair.second->onSpawn(player);
}

void SubsystemCaller::doKill(Player &player) {
  for (const auto &pair : arena.subsystems)
    pair.second->onKill(player);
}

void SubsystemCaller::doStartGame() {
  for (const auto &pair : arena.subsystems)
    pair.second->onStartGame();
}

void SubsystemCaller::doEndGame() {
  for (const auto &pair : arena.subsystems)
    pair.second->onEndGame();
}

/**
 * ArenaLogger.
 */

void ArenaLogger::onEvent(const ArenaEvent &) { }

ArenaLogger::ArenaLogger(Arena &arena) : arena(arena) {
  arena.loggers.push_back(this);
}

/**
 * Arena.
 */

ArenaInit::ArenaInit(
    const std::string &name,
    const EnvironmentURL &environment,
    const ArenaMode mode,
    const int teamCount) :
    name(name),
    environment(environment),
    mode(mode),
    teamCount(teamCount) { }

PID Arena::allocPid() const {
  return smallestUnused(players);
}

std::string Arena::allocNickname(const std::string &requestedNick,
                                 const optional<PID> ignorePid) const {
  std::string cleanReq(requestedNick);
  boost::algorithm::trim_right(cleanReq);

  std::stringstream readStream;
  PID nickNumber;
  const size_t rsize = cleanReq.size();
  std::vector<PID> usedNumbers;

  for (const auto &player : players) {
    if (ignorePid) {
      if (player.first == *ignorePid) continue;
    }

    const std::string &name = player.second.getNickname();
    if (name.size() < rsize) continue;
    if (name.substr(0, rsize) != cleanReq) continue;

    if (name == cleanReq) {
      usedNumbers.push_back(0);
      continue;
    }

    readStream.clear();
    readStream.str(name.substr(rsize));

    if (readStream.get() != '(') continue;
    readStream >> nickNumber;
    if (!readStream.good()) continue;
    if (readStream.get() != ')') continue;
    readStream.get();
    if (!readStream.eof()) continue;

    usedNumbers.push_back(nickNumber);
  }

  PID allocated = smallestUnused(usedNumbers);
  if (allocated == 0) return cleanReq;
  else
    return cleanReq + "("
        + std::to_string(smallestUnused(usedNumbers)) + ")";
}

void Arena::logEvent(const ArenaEvent &event) const {
  for (auto l : loggers) l->onEvent(event);
}

Player &Arena::joinPlayer(const PlayerInitializer &initializer) {
  if (Player *oldPlayer = getPlayer(initializer.pid)) quitPlayer(*oldPlayer);
  players.emplace(initializer.pid, Player(*this, initializer));
  Player &newPlayer = players.at(initializer.pid);
  for (auto s : subsystems) {
    s.second->registerPlayer(newPlayer);
    s.second->onJoin(newPlayer);
  }
  logEvent(ArenaEvent::Join(newPlayer.getNickname()));
  return newPlayer;
}

void Arena::quitPlayer(Player &player) {
  for (auto s : subsystems) {
    s.second->onQuit(player);
    s.second->unregisterPlayer(player);
  }
  logEvent(ArenaEvent::Quit(player.getNickname()));
  players.erase(player.pid);
}

void Arena::applyPlayerDelta(const PID pid, const PlayerDelta &playerDelta) {
  if (Player *player = getPlayer(pid)) {
    const auto oldNick = player->getNickname();
    const auto oldTeam = player->getTeam();

    player->applyDelta(playerDelta);

    const auto newNick = player->getNickname();
    const auto newTeam = player->getTeam();

    if (newNick != oldNick)
      logEvent(ArenaEvent::NickChange(oldNick, newNick));
    if (newTeam != oldTeam)
      logEvent(ArenaEvent::TeamChange(newNick, oldTeam, newTeam));

    for (auto &pair : subsystems) {
      pair.second->onDelta(*player, playerDelta);
    }
  }
}

Arena::Arena(const ArenaInit &initializer, const optional<PID> isClient, const bool isSandbox) :
    Networked(initializer),
    name(initializer.name),
    motd(initializer.motd),
    nextEnv(initializer.environment),
    mode(initializer.mode),
    uptime(0),
    debugTimer(2),
    teamCount(initializer.teamCount),
    role(isClient, isSandbox),
    subsystemCaller(*this) {
  for (auto const &player : initializer.players) {
    players.emplace(std::piecewise_construct,
                    std::forward_as_tuple(player.first),
                    std::forward_as_tuple(*this, player.second));
  }
}

void Arena::applyDelta(const ArenaDelta &delta) {
  switch (delta.type) {
    case ArenaDelta::Type::Quit: {
      if (Player *player = getPlayer(delta.quit.get())) {
        for (auto s : subsystems) s.second->onQuit(*player);
        quitPlayer(*player);
      }
      break;
    }

    case ArenaDelta::Type::Join: {
      Player &player = joinPlayer(delta.join.get());
      for (auto s : subsystems) s.second->onJoin(player);
      break;
    }

    case ArenaDelta::Type::Delta: {
      for (const auto &pair : delta.playerDeltas.get())
        applyPlayerDelta(pair.first, pair.second);
      break;
    }

    case ArenaDelta::Type::ResetEnvLoad: {
      forPlayers([&](Player &player) {
        player.loadingEnv = true;
      });
      break;
    }

    case ArenaDelta::Type::Motd: {
      motd = *delta.motd;
      break;
    }

    case ArenaDelta::Type::Mode: {
      if (mode != *delta.mode) {
        mode = *delta.mode;
        logEvent(ArenaEvent::ModeChange(mode));
        for (auto s : subsystems) s.second->onMode(mode);
      }
      break;
    }

    case ArenaDelta::Type::EnvChange: {
      nextEnv = *delta.environment;
      logEvent(ArenaEvent::EnvChoose(nextEnv));
      for (auto s : subsystems) s.second->onMapChange();
      break;
    }

    case ArenaDelta::Type::TeamCount: {
      teamCount = *delta.teamCount;
      break;
    }

  }
}

ArenaInit Arena::captureInitializer() const {
  ArenaInit initializer(name, nextEnv);
  for (auto &player : players) {
    initializer.players.emplace(
        player.first, player.second.captureInitializer());
  }
  initializer.motd = motd;
  initializer.mode = mode;
  return initializer;
}

Player *Arena::getPlayer(const PID pid) {
  auto player = players.find(pid);
  if (player != players.end()) return &player->second;
  return nullptr;
}

void Arena::forPlayers(std::function<void(const Player &)> f) const {
  for (const auto &pair : players) f(pair.second);
}

void Arena::forPlayers(std::function<void(Player &)> f) {
  for (auto &pair : players) f(pair.second);
}

const std::map<PID, Player> &Arena::getPlayers() const {
  return players;
}

const std::string &Arena::getName() const {
  return name;
}

const std::string &Arena::getMotd() const {
  return motd;
}

const EnvironmentURL &Arena::getNextEnv() const {
  return nextEnv;
}

ArenaMode Arena::getMode() const {
  return mode;
}

Time Arena::getUptime() const {
  return uptime;
}

int Arena::getTeamCount() const {
  return teamCount;
}

Team Arena::autojoinTeam() const {
  // TODO: Come back here when we have more than two teams.
  int reds = 0;
  int blues = 0;
  forPlayers([&reds, &blues](const sky::Player &player) {
    switch (player.getTeam()) {
      case sky::Team::Red:
        reds++;
        return;
      case sky::Team::Blue:
        blues++;
        return;
      case sky::Team::Spectator:
        return;
      default:
        return;
    }
  });

  if (reds < blues) {
    return sky::Team::Red;
  } else if (blues > reds) {
    return sky::Team::Blue;
  } else {
    return static_cast<sky::Team>((std::rand() % getTeamCount()) + 1);
  }
}

ArenaDelta Arena::connectPlayer(const std::string &requestedNick) {
  Player &player = joinPlayer(
      PlayerInitializer(allocPid(), allocNickname(requestedNick)));
  return ArenaDelta::Join(player.captureInitializer());
}

std::string Arena::allocNewNickname(const Player &player,
                                    const std::string &requestedNick) {
  return allocNickname(requestedNick, player.pid);
}

void Arena::poll() {
  for (auto s : subsystems) s.second->onPoll();
}

void Arena::tick(const TimeDiff delta) {
  uptime += Time(delta);
  if (debugTimer.tick(delta)) {
    for (auto s : subsystems) s.second->onDebugRefresh();
    debugTimer.reset();
  }
  for (auto s : subsystems) s.second->onTick(delta);
}

}
