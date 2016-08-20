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

    connectionStats(initializer.connectionStats),

    arena(arena),
    pid(initializer.pid) { }

void Player::applyDelta(const PlayerDelta &delta) {
  if (delta.nickname) nickname = *delta.nickname;
  admin = delta.admin;
  loadingEnv = delta.loadingEnv;
  if (delta.team) team = delta.team.get();
  if (delta.connectionStats) {
    connectionStats = delta.connectionStats.get();
  }
}

PlayerInitializer Player::captureInitializer() const {
  PlayerInitializer initializer;
  initializer.pid = pid;
  initializer.nickname = nickname;
  initializer.admin = admin;
  initializer.loadingEnv = loadingEnv;
  initializer.team = team;
  initializer.connectionStats = connectionStats;
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

PlayerConnectionStats const *Player::getConnectionStats() const {
  return connectionStats.get_ptr();
}

bool operator==(const Player &x, const Player &y) {
  return x.pid == y.pid;
}

bool operator!=(const Player &x, const Player &y) {
  return !(x == y);
}

}
