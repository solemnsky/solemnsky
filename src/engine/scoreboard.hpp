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
/**
 * Scoreboard, carrying score data for each player.
 */
#pragma once
#include "arena.hpp"

namespace sky {

using ScoreRecordInit = std::vector<int>;
using ScoreRecordDelta = std::vector<int>;

/**
 * Score record, describing the score of a single player.
 * This is meant to be very flexible; the meanings and nature of the scores
 * are configurable at runtime.
 */
struct ScoreRecord
    : public Networked<ScoreRecordInit, ScoreRecordDelta> {
 private:
  std::vector<int> lastValues;

 public:
  ScoreRecord(const ScoreRecordInit &init);

  // State.
  std::vector<int> values;

  // Networked impl.
  void applyDelta(const ScoreRecordDelta &delta) override final;
  ScoreRecordInit captureInitializer() const override final;
  optional<ScoreRecordDelta> collectDelta();

  // User API.
  int getValueAt(const size_t index);
  void setValueAt(const size_t index, const int value);

};

/**
 * Initializer for Scoreboard.
 */
struct ScoreboardInit {
  ScoreboardInit() = default;
  ScoreboardInit(const std::vector<std::string> &fields);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(fields, initializers);
  }

  std::vector<std::string> fields;
  std::map<PID, ScoreRecordInit> initializers;

};

/**
 * Delta for Scoreboard.
 */
struct ScoreboardDelta {
  ScoreboardDelta();

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(fields, deltas);
  }

  optional<std::vector<std::string>> fields;
  std::map<PID, ScoreRecordDelta> deltas;

};

/**
 * The scoreboard subsystem.
 */
class Scoreboard
    : public Subsystem<ScoreRecord>,
      public Networked<ScoreboardInit, ScoreboardDelta> {
 private:
  // State.
  std::vector<std::string> fields;
  std::map<PID, ScoreRecord> records;

  std::vector<std::string> lastFields;

 protected:
  void registerPlayerWith(Player &player, const ScoreRecordInit &initializer);
  // Subsystem impl.
  void registerPlayer(Player &player) override final;
  void unregisterPlayer(Player &player) override final;

 public:
  Scoreboard(Arena &arena, const ScoreboardInit &initializer);

  // Networked impl.
  void applyDelta(const ScoreboardDelta &delta) override final;
  ScoreboardInit captureInitializer() const override final;
  optional<ScoreboardDelta> collectDelta();

  // User API.
  const std::vector<std::string> &getFields();
  ScoreRecord &getScore(const Player &player);
  const ScoreRecord &getScoreRecord(const Player &player) const;

};

}
