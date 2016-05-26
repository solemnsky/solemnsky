#include "latencytracker.h"

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

sky::ArenaDelta LatencyTracker::makeUpdate() {
  std::map<PID, sky::PlayerDelta> deltas;
  arena.forPlayers([&](sky::Player &player) {
    sky::PlayerDelta delta(player.zeroDelta());
    auto latency = getPlayerData(player);
    delta.latency = latency.getLatency();
    delta.clockOffset = latency.getOffset();
    deltas.emplace(player.pid, delta);
  });
  return sky::ArenaDelta::Delta(deltas);
}

