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
 * Buffer timestamped message streams, pulling from them with constant timeflow rate.
 */
#pragma once
#include <queue>
#include "util/types.hpp"
#include "util/printer.hpp"

namespace sky {

/**
 * Statistics about the operation of a FlowControl.
 */
struct FlowStats {
  FlowStats() = default;
  FlowStats(const TimeDiff averageWait, const TimeDiff totalJitter) :
      averageWait(averageWait), totalJitter(totalJitter) { }

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(averageWait, totalJitter);
  }

  TimeDiff averageWait, totalJitter;

};

/**
 * Manages statistics of a flow, resulting in a decision procedure for releasing messages from the cache.
 */
class FlowState {
 private:
  RollingSampler<Time> offsets;
  optional<Time> window;

 public:
  FlowState();

  void registerArrival(const Time offset);
  bool release(const Time offset);

};

/**
 * A message, along with the localtime it arrived and a timestamp from upstream.
 */
template<typename Message>
struct TimedMessage {
  TimedMessage(const Message &message,
               const Time arrivalTime,
               const Time timestamp) :
      message(message), arrivalTime(arrivalTime), timestamp(timestamp) { }

  Message message;
  Time arrivalTime, timestamp;
};

/**
 * FlowControl class, templated on the Message type. Push messages in chronological order and pull them.
 *
 * Very simply, the objective is to make the `pull` method return messages
 * with timestamps equidistant to the supplied localtimes.
 */
template<typename Message>
class FlowControl {
 private:
  std::queue<TimedMessage<Message>> messages;
  FlowState flowState;

  RollingSampler<TimeDiff> waitingTime;
  RollingSampler<Time> offsets;

 public:
  FlowControl() :
      waitingTime(50),
      offsets(50) { }

  // A message with a timestamp arrives.
  void registerMessage(const Time localtime, const Time timestamp, const Message &message) {
    flowState.registerArrival(localtime - timestamp);
    messages.push(TimedMessage<Message>(message, localtime, timestamp));
  }
  void registerArrival(const Time localtime, const Time timestamp) {
    flowState.registerArrival(localtime - timestamp);
  }

  // Potentially pull a message, given the current localtime.
  optional<Message> pull(const Time localtime) {
    if (!messages.empty()) {
      const Time difference = localtime - messages.front().timestamp;
      if (flowState.release(difference)) {
        const auto msg = messages.front();
        waitingTime.push(TimeDiff(localtime - msg.arrivalTime));
        offsets.push(localtime - msg.timestamp);
        messages.pop();
        return {msg.message};
      }
    }

    return {};
  }

  // Reset the flow, discarding all messages.
  void reset() {
    if (!messages.empty()) messages = std::queue<TimedMessage<Message>> ();
  }

  // Capture rolling statistics.
  FlowStats getStats() const {
    return FlowStats(waitingTime.mean<TimeDiff>(),
                     TimeDiff(offsets.max() - offsets.min()));
  }

};

}

