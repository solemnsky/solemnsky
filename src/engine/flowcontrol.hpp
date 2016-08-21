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
 * Decide reconciliation delays for a stream of timestamped updates.
 */
#pragma once
#include <queue>
#include "util/types.hpp"

namespace sky {

/**
 * Statistics about the operation of a FlowControl.
 */
struct FlowStats {
  FlowStats() = default;
  FlowStats(const TimeDiff inputJitter,
            const TimeDiff maxDelay,
            const TimeDiff minDelay);

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(inputJitter, maxDelay, minDelay);
  }

  TimeDiff inputJitter, maxDelay, minDelay;

  void printDebug(class Printer &p) const;

};

/**
 *
 */
class FlowControl {
 private:
  RollingSampler<Time> offsets;

 public:
  FlowControl();

  // Given a localtime that an update with a certain timestamp was received, determine a delay to reconcile with.
  TimeDiff determineDelay(const Time localtime, const Time timestamp);

  FlowStats getStats() const;

};

}

