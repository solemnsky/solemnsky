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
#include "player.hpp"
#include "arena.hpp"

namespace sky {

/**
 * PlayerDelta.
 */

PlayerDelta::PlayerDelta(const Player &player) :
    admin(player.isAdmin()),
    loadingEnv(player.isLoadingEnv()) { }

/**
 * Player.
 */

PlayerInitializer::PlayerInitializer(
    const PID pid, const std::string &nickname) :
    pid(pid), nickname(nickname), admin(false),
    loadingEnv(true), team(sky::Team::Spectator) { }

Player::Player(Arena &arena, const PlayerInitializer &initializer) :
    Networked(initializer),
    nickname(initializer.nickname),
    admin(initializer.admin),
    team(initializer.team),
    loadingEnv(initializer.loadingEnv),

    latencyInitialized(bool(initializer.latencyStats)),
    latency(initializer.latencyStats
            ? initializer.latencyStats->first : 0),
    clockOffset(initializer.latencyStats
                ? initializer.latencyStats->second : 0),

    arena(arena),
    pid(initializer.pid) { }

void Player::applyDelta(const PlayerDelta &delta) {
  if (delta.nickname) nickname = *delta.nickname;
  admin = delta.admin;
  loadingEnv = delta.loadingEnv;
  if (delta.team) team = delta.team.get();
  if (delta.latencyStats) {
    latency = delta.latencyStats->first;
    clockOffset = delta.latencyStats->second;
    latencyInitialized = true;
  }
}

PlayerInitializer Player::captureInitializer() const {
  PlayerInitializer initializer;
  initializer.pid = pid;
  initializer.nickname = nickname;
  initializer.admin = admin;
  initializer.loadingEnv = loadingEnv;
  initializer.team = team;
  if (latencyInitialized) {
    initializer.latencyStats.emplace(latency, clockOffset);
  }
  return initializer;
}

std::string Player::getNickname() const {
  return nickname;
}

bool Player::isAdmin() const {
  return admin;
}

Team Player::getTeam() const {
  return team;
}

bool Player::isLoadingEnv() const {
  return loadingEnv;
}

bool Player::latencyIsCalculated() const {
  return latencyInitialized;
}

TimeDiff Player::getLatency() const {
  return latency;
}

Time Player::getClockOffset() const {
  return clockOffset;
}

bool operator==(const Player &x, const Player &y) {
  return x.pid == y.pid;
}

bool operator!=(const Player &x, const Player &y) {
  return !(x == y);
}

}
