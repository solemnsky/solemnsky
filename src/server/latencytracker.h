/**
 * Latency recording subsystem for use by ServerExec.
 */
#pragma once

#include "sky/arena.h"
#include "util/types.h"

/**
 * Latency data for a player.
 */
struct PlayerLatency {
 private:
  RollingSampler<Time> latencySampler, offsetSampler;
 public:
  PlayerLatency() = default;

  void registerPong(const Time now,
                    const Time pingTime,
                    const Time pongTime);

  Time getLatency();
  Time getOffset();

};

class LatencyTracker: public sky::Subsystem<PlayerLatency> {
 private:
  std::map<PID, PlayerLatency> latencies;

 protected:

 public:
  LatencyTracker(sky::Arena &arena);

  void registerPong(const Player &player,
                    const Time now,
                    const Time pingTime,
                    const Time pongTime);

  sky::ArenaDelta makeUpdate();

};
