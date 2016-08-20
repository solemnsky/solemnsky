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
 * Representation of a player in an Arena.
 */
#pragma once
#include "util/types.hpp"
#include "types.hpp"
#include "sky/planestate.hpp"
#include "util/networked.hpp"
#include "flowcontrol.hpp"

namespace sky {

/**
 * Initializer type for Player's Networked implementation.
 */
struct PlayerInitializer {
  PlayerInitializer() = default; // packing
  PlayerInitializer(const PID pid, const std::string &nickname);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, pid, admin, loadingEnv, team, flowStats);
  }

  PID pid;
  std::string nickname;
  bool admin, loadingEnv;
  Team team;
  optional<std::pair<TimeDiff, Time>> latencyStats;
  optional<FlowStats> flowStats;

};

/**
 * Delta type for Player's Networked implementation.
 */
struct PlayerDelta {
  PlayerDelta() = default; // packing, do not use
  PlayerDelta(const class Player &player);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, admin, loadingEnv, team, latencyStats, flowStats);
  }

  optional<std::string> nickname;
  bool admin, loadingEnv;
  optional<Team> team;
  optional<std::pair<TimeDiff, Time>> latencyStats;
  optional<optional<FlowStats>> flowStats;

};

/**
 * Represents a player in the arena, with some minimal metadata.
 */
class Player: public Networked<PlayerInitializer, PlayerDelta> {
  template<typename T>
  friend
  class Subsystem;
  friend class Arena;
 private:
  // State.
  std::string nickname; // Nickname allocated for player.
  bool admin; // Player has admin rights?
  Team team;
  bool loadingEnv; // Player is in process of loading environment?

  // Subsystem state.
  std::map<PID, void *> data;

  // Connection statistics.
  bool latencyInitialized;
  TimeDiff latency;
  Time clockOffset;
  optional<FlowStats> flowStats;

 public:
  Player() = delete;
  Player(class Arena &arena, const PlayerInitializer &initializer);

  // Parameters.
  class Arena &arena;
  const PID pid;

  // Networked impl.
  void applyDelta(const PlayerDelta &delta) override;
  PlayerInitializer captureInitializer() const override;

  // User API.
  std::string getNickname() const;
  bool isAdmin() const;
  Team getTeam() const;

  // (This only has meaning when we're in a game.)
  bool isLoadingEnv() const;

  bool latencyIsCalculated() const;
  TimeDiff getLatency() const;
  Time getClockOffset() const;

};

/**
 * Comparison by PID.
 */
bool operator==(const Player &, const Player &);
bool operator!=(const Player &, const Player &);

}
