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
#include "skyinputcache.hpp"

namespace sky {

/**
 * PlayerInputCache.
 */
PlayerInputCache::PlayerInputCache(sky::Player &player, ServerShared &shared) :
    player(player), shared(shared), arena(shared.arena), inputControl({}) { }

void PlayerInputCache::cacheInput(const Time timestamp,
                                  const sky::ParticipationInput &input) {
  inputControl.registerMessage(arena.getUptime(), timestamp, input);
}

void PlayerInputCache::poll() {
  if (const auto sky = shared.skyHandle.getSky()) {
    while (const auto &input = inputControl.pull(arena.getUptime())) {
      sky->getParticipation(player).applyInput(*input);
    }
  } else {
    inputControl.reset();
  }
}

void SkyInputCache::registerPlayer(sky::Player &player) {
  inputManagers.emplace(std::piecewise_construct,
                        std::forward_as_tuple(player.pid),
                        std::forward_as_tuple(player, shared));
  setPlayerData(player, inputManagers.at(player.pid));
}

void SkyInputCache::unregisterPlayer(sky::Player &player) {
  inputManagers.erase(inputManagers.find(player.pid));
}

void SkyInputCache::onPoll() {
  for (auto &manager : inputManagers) {
    manager.second.poll();
  }
}

SkyInputCache::SkyInputCache(ServerShared &shared) :
    Subsystem(shared.arena), shared(shared) { }

void SkyInputCache::receive(
    sky::Player &player, const Time timestamp,
    const sky::ParticipationInput &input) {
  getPlayerData(player).cacheInput(timestamp, input);
}

}
