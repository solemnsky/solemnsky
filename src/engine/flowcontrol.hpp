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
struct FlowControlState {
  FlowControlState(); // initialize with agnostic defaults

  optional<Time> windowEntry;
  TimeDiff windowSize;

};

namespace detail {

// Decide whether to pull a message with a certain timestamp, given a
bool pullMessage(const FlowControlState &state,
                 const Time localtime, const Time timestamp);

}

/**
 * FlowControl class, templated on the Message type. Push messages in chronological order and pull them.
 *
 * Very simply, the objective is to make the `pull` method return messages with timestamps
 * equidistant to the supplied localtime.
 */
template<typename Message>
class FlowControl {
 private:
  const FlowControlState settings;
  std::queue<std::pair<Time, Message>> messages;
  FlowControlState state;

 public:
  FlowControl(const FlowControlState &state) : state(state) {}

  // A message with a timestamp arrives.
  void push(const Time timestamp, Message &&message) {
    messages.emplace(std::piecewise_construct, timestamp, message);
  }

  // Potentially pull a message, given the current localtime.
  optional<Message> pull(const Time localtime) {
    if (!messages.empty()) {
      if (detail::pullMessage(state, localtime, messages.front().first)) {
        const Message msg = messages.front().second;
        messages.pop();
        return {msg};
      }
    } else {
      return {};
    }
  }

};

}
