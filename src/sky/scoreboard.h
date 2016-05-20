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
#include "arena.h"

namespace sky {

/**
 * Initializer for ScoreRecord.
 */
struct ScoreRecordInit {
  ScoreRecordInit() = default;

  template<typename Archive>
  void serialize(Archive &ar) { }

};

/**
 * Delta for ScoreRecord.
 */
struct ScoreRecordDelta {
  ScoreRecordDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) { }

};

/**
 * Score record, describing the score of a single player.
 * This is meant to be very flexible; the meanings and nature of the scores
 * is configurable at runtime.
 */
struct ScoreRecord :
    public Networked<ScoreRecordInit, ScoreRecordDelta> {
  ScoreRecord() = default;
  ScoreRecord(const ScoreRecordInit &init);

  // Networked impl.
  void applyDelta(const ScoreRecordDelta &delta) override final;
  ScoreRecordInit captureInitializer() const override final;

};

/**
 * Initializer for Scoreboard.
 */
struct ScoreboardInitializer {
  ScoreboardInitializer() = default;

  template<typename Archive>
  void serialize(Archive &ar) { }

};

/**
 * Delta for Scoreboard.
 */

/**
 * The scoreboard subsystem.
 */
class Scoreboard: public Subsystem<ScoreRecord> {
 private:
  // State.
  std::map<PID, ScoreRecord> records;

 protected:
  // Subsystem impl.
  void registerPlayer(Player &player) override final;
  void unregisterPlayer(Player &player) override final;

 public:
  Scoreboard(Arena &arena);

  // Networked impl.

  // User API.
  ScoreRecord &getScoreRecord(const Player &player);
  const ScoreRecord &getScoreRecord(const Player &player) const;

};

}
