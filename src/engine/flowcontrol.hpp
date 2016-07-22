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

namespace sky {

/**
 * State of a FlowControl, governing what messages to pull.
 */
struct FlowControlSettings {
  FlowControlSettings(); // agnostic defaults

  optional<Time> windowEntry;
  TimeDiff windowSize;

};

namespace detail {

// Decide whether to pull a message given the flow settings and a
// localtime / timestamp pair.
bool pullMessage(const FlowControlSettings &settings,
                 const Time localtime, const Time timestamp);

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

 public:
  FlowControl(const FlowControlSettings &settings) :
      settings(settings) { }

  FlowControlSettings settings;

  // A message with a timestamp arrives.
  void push(const Time timestamp, const Message &message) {
    messages.push({timestamp, message});
  }

  // Potentially pull a message, given the current localtime.
  optional<Message> pull(const Time localtime) {
    if (!messages.empty()) {
      if (detail::pullMessage(settings, localtime, messages.front().first)) {
        const Message msg = messages.front().second;
        messages.pop();
        return {msg};
      }
    }
    return {};
  }

};

}
