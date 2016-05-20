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
#include "scoreboard.h"

namespace sky {

/**
 * ScoreRecord.
 */



/**
 * Scoreboard.
 */

void Scoreboard::registerPlayer(Player &player) {
  records.emplace(player.pid, {});
  setPlayerData(player, records[player.pid]);
}

void Scoreboard::unregisterPlayer(Player &player) {
  records.erase(records.find(player.pid));
}

Scoreboard::Scoreboard(Arena &arena) :
  Subsystem(arena) {
  arena.forPlayers([&](Player &player) {
    registerPlayer(player);
  });
}


ScoreRecord &Scoreboard::getScoreRecord(const Player &player) {
  return getPlayerData(player);
}

const ScoreRecord &Scoreboard::getScoreRecord(const Player &player) const {
  return getPlayerData(player);
}

}
