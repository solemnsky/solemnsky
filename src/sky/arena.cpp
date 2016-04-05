#include "arena.h"
#include "event.h"

namespace sky {

/**
 * Player.
 */

PlayerInitializer::PlayerInitializer(
    const PID pid, const std::string &nickname) :
    pid(pid), nickname(nickname), admin(false), team(0) { }

Player::Player(Arena &arena, const PlayerInitializer &initializer) :
    Networked(initializer),
    arena(arena),
    pid(initializer.pid),
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
  initializer.pid = pid;
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

void Player::doAction(const Action action, const bool state) {
  arena.onAction(*this, action, state);
}

void Player::spawn(const PlaneTuning &tuning,
                   const sf::Vector2f &pos,
                   const float rot) {
  arena.onSpawn(*this, tuning, pos, rot);
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

ArenaDelta ArenaDelta::Join(const PlayerInitializer &initializer) {
  ArenaDelta delta(Type::Join);
  delta.join = initializer;
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

SubsystemCaller::SubsystemCaller(Arena &arena) : arena(arena) { }

void SubsystemCaller::kill(Player &player) {
  for (auto s : arena.subsystems) s->onKill(player);
}

Subsystem::Subsystem(Arena &arena) :
    arena(arena),
    caller(arena.caller),
    id(PID(arena.subsystems.size())) {
  arena.subsystems.push_back(this);
}

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
  std::vector<int> usedNumbers;

  for (const auto &player : players) {
    const std::string &name = player.second.nickname;
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

  int allocated = smallestUnused(usedNumbers);
  if (allocated == 0) return requested;
  else
    return requested + "("
        + std::to_string(smallestUnused(usedNumbers)) + ")";
}

void Arena::onAction(Player &player, const Action action, const bool state) {
  for (auto s : subsystems) s->onAction(player, action, state);
}

void Arena::onSpawn(Player &player, const PlaneTuning &tuning,
                    const sf::Vector2f &pos, const float rot) {
  for (auto s : subsystems) s->onSpawn(player, tuning, pos, rot);
}

void Arena::onDie(Player &player) {
  for (auto s : subsystems) s->onKill(player);
}

void Arena::onEvent(const ArenaEvent &event) const {
  for (auto s : subsystems) s->onEvent(event);
}

Arena::Arena(const ArenaInitializer &initializer) :
    Networked(initializer),
    caller(*this),
    name(initializer.name),
    motd(initializer.motd),
    mode(initializer.mode) {
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
        quitPlayer(*player);
      }
      break;
    }

    case ArenaDelta::Type::Join: {
      joinPlayer(delta.join.get());
      break;
    }

    case ArenaDelta::Type::Delta: {
      const PID &pid = delta.delta->first;
      const PlayerDelta &playerDelta = delta.delta->second;

      if (Player *player = getPlayer(pid)) {
        std::string oldNick = player->nickname;
        Team oldTeam = player->team;

        player->applyDelta(playerDelta);

        if (player->nickname != oldNick)
          onEvent(ArenaEvent::NickChange(oldNick, player->nickname));
        if (player->team != oldTeam)
          onEvent(ArenaEvent::TeamChange(
              player->nickname, oldTeam, player->team));
      }
      break;
    }

    case ArenaDelta::Type::Motd: {
      motd = *delta.motd;
      break;
    }

    case ArenaDelta::Type::Mode: {
      if (mode != *delta.mode) {
        onEvent(ArenaEvent::ModeChange(mode));
        mode = *delta.mode;
      }
      break;
    }
  }
}

ArenaInitializer Arena::captureInitializer() const {
  ArenaInitializer initializer;
  for (auto &player : players) {
    initializer.players.emplace(
        player.first, player.second.captureInitializer());
  }
  initializer.name = name;
  initializer.motd = motd;
  initializer.mode = mode;
  return initializer;
}

Player &Arena::joinPlayer(const PlayerInitializer &initializer) {
  if (Player *oldPlayer = getPlayer(initializer.pid)) quitPlayer(*oldPlayer);
  players.emplace(initializer.pid, Player(*this, initializer));
  Player &newPlayer = players.at(initializer.pid);
  for (auto s : subsystems) {
    s->registerPlayer(newPlayer);
    s->onJoin(newPlayer);
    s->onEvent(ArenaEvent::Join(newPlayer.nickname));
  }
  return newPlayer;
}

void Arena::quitPlayer(Player &player) {
  for (auto s : subsystems) {
    s->onQuit(player);
    s->onEvent(ArenaEvent::Quit(player.nickname));
    s->unregisterPlayer(player);
  }
  players.erase(player.pid);
}

Player &Arena::connectPlayer(const std::string &requestedNick) {
  return joinPlayer(
      PlayerInitializer(allocPid(), allocNickname(requestedNick)));
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

void Arena::tick(const float delta) {
  for (auto s : subsystems) s->onTick(delta);
}

}
