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
 * Manages statistics of a flow, resulting in a decision procedure for releasing messages from the cache.
 */
class FlowState {
 public:
  void registerArrival(const Time offset);
  bool release(const Time offset);

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
  std::queue<std::pair<Time, Message>> messages;
  FlowState flowState;

 public:
  FlowControl() = default;

  // A message with a timestamp arrives.
  void registerMessage(const Time localtime, const Time timestamp, const Message &message) {
    flowState.registerArrival(localtime - timestamp);
    messages.push({timestamp, message});
  }

  void registerArrival(const Time localtime, const Time timestamp) {
    flowState.registerArrival();
  }

  // Potentially pull a message, given the current localtime.
  optional<Message> pull(const Time localtime) {
    if (!messages.empty()) {
      const Time difference = localtime - messages.front().first;
      if (flowState.release(difference)) {
        const auto msg = messages.front().second;
        messages.pop();
        return {msg};
      }
    }

    return {};
  }

  void reset() {
    if (!messages.empty()) messages = std::queue<std::pair<Time, Message>>();
  }

};

}
