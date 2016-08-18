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
#include "skyinputmanager.hpp"


/**
 * PlayerInputManager.
 */
PlayerInputManager::PlayerInputManager(sky::Player &player, ServerShared &shared) :
    player(player), shared(shared), arena(shared.arena), inputControl({}) { }

void PlayerInputManager::poll() {
  while (const auto &input = inputControl.pull(arena.getUptime())) {
    if (const auto sky = shared.skyHandle.getSky()) {
      sky->getParticipation(player).applyInput(*input);
    }
  }
}

void SkyInputManager::registerPlayer(sky::Player &player) {
  inputManagers.emplace(std::piecewise_construct,
                        std::forward_as_tuple(player.pid),
                        std::forward_as_tuple(player, shared));
}

void SkyInputManager::unregisterPlayer(sky::Player &player) {
  inputManagers.erase(inputManagers.find(player.pid));
}

void SkyInputManager::onTick()

SkyInputManager::SkyInputManager(ServerShared &shared) :
    Subsystem(shared.arena), shared(shared) { }
