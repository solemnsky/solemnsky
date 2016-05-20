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
 * ScoreRecordInit.
 */

ScoreboardInit::ScoreboardInit(
    const std::vector<bool> &fields) :
    fields(fields) { }

/**
 * ScoreRecord.
 */


ScoreRecord::ScoreRecord(const ScoreRecordInit &init) :
    Networked(init),
    values(init) { }

void ScoreRecord::applyDelta(const ScoreRecordDelta &delta) {
  if (delta) {
    values = delta.get();
  }
}

ScoreRecordInit ScoreRecord::captureInitializer() const {
  return values;
}

ScoreRecordDelta ScoreRecord::collectDelta() {
  if (lastValues == values) return {};
  else {
    lastValues = values;
    return values;
  }
}

int ScoreRecord::getValueAt(const size_t index) {
  if (index < values.size()) return values.at(index);
  else return 0;
}

void ScoreRecord::setValueAt(const size_t index, const int value) {
  values[index] = value;
}


/**
 * Scoreboard.
 */


void Scoreboard::registerPlayerWith(Player &player,
                                    const ScoreRecordInit &initializer) {
  records.emplace(player.pid, initializer);
  setPlayerData(player, records.at(player.pid));
}


void Scoreboard::registerPlayer(Player &player) {
  registerPlayerWith(player, {});
}

void Scoreboard::unregisterPlayer(Player &player) {
  records.erase(records.find(player.pid));
}

Scoreboard::Scoreboard(Arena &arena,
                       const ScoreboardInit &initializer) :
    Networked(initializer),
    Subsystem(arena),
    fields(initializer.fields) {
  arena.forPlayers([&](Player &player) {
    registerPlayer(player);
  });
}

void Scoreboard::applyDelta(const ScoreboardDelta &delta) {

}

ScoreboardInit Scoreboard::captureInitializer() const {
  return sky::ScoreboardInit();
}

const std::vector<std::string> &Scoreboard::getFields() {
  return fields;
}

ScoreRecord &Scoreboard::getScoreRecord(const Player &player) {
  return getPlayerData(player);
}

const ScoreRecord &Scoreboard::getScoreRecord(const Player &player) const {
  return getPlayerData(player);
}

}
