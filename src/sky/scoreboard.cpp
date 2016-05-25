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

ScoreRecord::ScoreRecord(const ScoreRecordInit &init) :
    Networked(init),
    values(init) { }

void ScoreRecord::applyDelta(const ScoreRecordDelta &delta) {
  values = delta;
}

ScoreRecordInit ScoreRecord::captureInitializer() const {
  return values;
}

optional<ScoreRecordDelta> ScoreRecord::collectDelta() {
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
  if (values.size() <= index) values.resize(index + 1);
  values.at(index) = value;
}

/**
 * ScoreboardInit.
 */

ScoreboardInit::ScoreboardInit(
    const std::vector<std::string> &fields) :
    fields(fields) { }

/**
 * ScoreboardDelta.
 */

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
    Subsystem(arena),
    Networked(initializer),
    fields(initializer.fields),
    lastFields(fields) {
  arena.forPlayers([&](Player &player) {
    const auto iter = initializer.initializers.find(player.pid);
    if (iter != initializer.initializers.end())
      registerPlayerWith(player, iter->second);
    else registerPlayerWith(player, {});
  });
}

void Scoreboard::applyDelta(const ScoreboardDelta &delta) {
  if (delta.fields) {
    fields = delta.fields.get();
  }
  for (const auto &pair : delta.deltas) {
    const auto iter = records.find(pair.first);
    if (iter != records.end())
      iter->second.applyDelta(pair.second);
  }
}

ScoreboardInit Scoreboard::captureInitializer() const {
  ScoreboardInit init;
  init.fields = fields;
  for (const auto &pair : records) {
    init.initializers.emplace(pair.first, pair.second.captureInitializer());
  }
  return init;
}

optional<ScoreboardDelta> Scoreboard::collectDelta() {
  bool isUseful{false};
  ScoreboardDelta delta;

  if (fields != lastFields) {
    delta.fields = fields;
    lastFields = fields;
    isUseful = true;
  }
  for (auto &pair : records) {
    if (const auto recordDelta = pair.second.collectDelta()) {
      isUseful = true;
      delta.deltas.emplace(pair.first, recordDelta.get());
    }
  }

  if (isUseful) return delta;
  else return {};
}

const std::vector<std::string> &Scoreboard::getFields() {
  return fields;
}

ScoreRecord &Scoreboard::getScore(const Player &player) {
  return getPlayerData(player);
}

const ScoreRecord &Scoreboard::getScoreRecord(const Player &player) const {
  return getPlayerData(player);
}

}
