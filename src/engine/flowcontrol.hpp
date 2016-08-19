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
 * State of a FlowControl, governing what messages to pull.
 */
struct FlowControlSettings {
  FlowControlSettings(); // null defaults
  FlowControlSettings(const optional<Time> windowEntry,
                      const optional<Time> windowSize);

  optional<Time> windowEntry; // Pull when localtime is ahead of timestamp by this amount.
  optional<TimeDiff> windowSize; // Discard messages that have passed this allowance period.

  // ---------------------- remote time
  // message ^
  //         -------------- localtime - timestamp (+ offset)
  //            reception ^
  // ---------------------- local time

  // This caches messages so that all receptions have (localtime - timestamp (+ offset) >= windowEntry
  // (+ offset)) and do not exceed this value + windowSize.


};

namespace detail {

// Decide whether to pull a message given the flow settings and a
// localtime / timestamp pair.
bool pullMessage(const FlowControlSettings &settings,
                 const Time difference); // difference = localtime - timestamp

}

/**
 * FlowControl class, templated on the Message type. Push messages in chronological order and pull them.
 *
 * Very simply, the objective is to make the `pull` method return messages
 * with timestamps equidistant to the supplied localtimes.
 */
template<typename Message>
class FlowControl {
 private:
  std::queue<std::pair<Time, Message>> messages;
  RollingSampler<Time> actualDifference;

 public:
  FlowControl(const FlowControlSettings &settings) :
    actualDifference(20), settings(settings) { }

  FlowControlSettings settings;

  // A message with a timestamp arrives.
  void push(const Time timestamp, const Message &message) {
    messages.push({timestamp, message});
  }

  // Potentially pull a message, given the current localtime.
  optional<Message> pull(const Time localtime) {
    if (!messages.empty()) {
      const Time difference = localtime - messages.front().first;
      if (detail::pullMessage(settings, difference)) {
        // Record the actual difference.
        actualDifference.push(difference);
        appLog(printTime(difference));

        const Message msg = messages.front().second;
        messages.pop();
        return {msg};
      }
    }
    return {};
  }

  void reset() {
    if (!messages.empty()) messages = std::queue<std::pair<Time, Message>>();
  }

  Time meanDifference() const {
    return actualDifference.mean<Time>();
  }

};

}
