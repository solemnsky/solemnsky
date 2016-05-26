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
