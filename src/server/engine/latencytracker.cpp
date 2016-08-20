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
#include "latencytracker.hpp"
#include "skyinputcache.hpp"

namespace sky {

/**
 * PlayerLatency.
 */

PlayerLatency::PlayerLatency() :
    latencySampler(5),
    offsetSampler(5) { }

void PlayerLatency::registerPong(const Time now,
                                 const Time pingTime,
                                 const Time pongTime) {
  latencySampler.push(TimeDiff(now - pingTime));
  offsetSampler.push(pongTime - (pingTime + (getLatency() / 2)));
}

TimeDiff PlayerLatency::getLatency() {
  return latencySampler.mean<TimeDiff>();
}

Time PlayerLatency::getOffset() {
  return offsetSampler.mean<Time>();
}

/**
 * LatencyTracker.
 */
void LatencyTracker::registerPlayer(sky::Player &player) {
  latencies.emplace(std::piecewise_construct,
                    std::forward_as_tuple(player.pid),
                    std::forward_as_tuple());
  setPlayerData(player, latencies.find(player.pid)->second);
}

void LatencyTracker::unregisterPlayer(sky::Player &player) {
  latencies.erase(latencies.find(player.pid));
}

LatencyTracker::LatencyTracker(sky::Arena &arena) :
    sky::Subsystem<PlayerLatency>(arena) {
  arena.forPlayers([&](sky::Player &player) {
    registerPlayer(player);
  });
}

void LatencyTracker::registerPong(const sky::Player &player,
                                  const Time pingTime,
                                  const Time pongTime) {
  getPlayerData(player).registerPong(arena.getUptime(), pingTime, pongTime);
}

ArenaDelta LatencyTracker::makeUpdate(SkyInputCache &cache) const {
  std::map<PID, sky::PlayerDelta> deltas;
  arena.forPlayers([&](sky::Player &player) {
    sky::PlayerDelta playerDelta{player};
    auto &playerData = getPlayerData(player);

	sky::PlayerConnectionStats stats;
    stats.latency = playerData.getLatency();
    stats.offset = playerData.getOffset();
    stats.flow = cache.getPlayerStats(player);

    playerDelta.connectionStats = stats;
    deltas.emplace(player.pid, playerDelta);
  });
  return sky::ArenaDelta::Delta(deltas);
}

}
