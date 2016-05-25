#include "latencytracker.h"

/**
 * PlayerLatency.
 */

void PlayerLatency::registerPong(const Time now,
                                 const Time pingTime,
                                 const Time pongTime) {
  latencySampler.push(now - pingTime);
  offsetSampler.push(Time(
      double(pongTime) - pingTime + (double(getLatency()) / 2.0f)));
}

Time PlayerLatency::getLatency() {
  return Time(latencySampler.mean<double>());
}

Time PlayerLatency::getOffset() {
  return Time(offsetSampler.mean<double>());
}

/**
 * LatencyTracker.
 */
LatencyTracker::LatencyTracker(sky::Arena &arena) :
    sky::Subsystem(arena) {
  arena.forPlayers([&](Player &player) {
    registerPlayer(player);
  });
}

void LatencyTracker::registerPong(const sky::Player &player,
                                  const Time now,
                                  const Time pingTime,
                                  const Time pongTime) {
  getPlayerData(player).registerPong(now, pingTime, pongTime);
}

sky::ArenaDelta LatencyTracker::makeUpdate() {
  std::map<PID, sky::PlayerDelta> deltas;
  arena.forPlayers([&](Player &player) {
    sky::PlayerDelta delta(player.zeroDelta());
    auto latency = getPlayerData(player);
    delta.latency = latency.getLatency();
    delta.clockOffset = latency.getOffset();
  });
  return sky::ArenaDelta::Delta(deltas);
}

