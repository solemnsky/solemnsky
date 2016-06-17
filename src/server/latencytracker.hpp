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
 * Latency recording subsystem for use by ServerExec.
 */
#pragma once
#include "sky/arena.hpp"
#include "util/types.hpp"

/**
 * Latency data for a player.
 */
struct PlayerLatency {
 private:
  RollingSampler<TimeDiff> latencySampler;
  RollingSampler<Time> offsetSampler;

 public:
  PlayerLatency();

  void registerPong(const Time now,
                    const Time pingTime,
                    const Time pongTime);

  TimeDiff getLatency();
  Time getOffset();

};

class LatencyTracker: public sky::Subsystem<PlayerLatency> {
 private:
  std::map<PID, PlayerLatency> latencies;

 protected:
  void registerPlayer(sky::Player &player) override final;
  void unregisterPlayer(sky::Player &player) override final;

 public:
  LatencyTracker(sky::Arena &arena);

  void registerPong(const sky::Player &player,
                    const Time pingTime,
                    const Time pongTime);

  sky::ArenaDelta makeUpdate();

};
