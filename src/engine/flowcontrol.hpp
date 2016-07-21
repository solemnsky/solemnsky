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
 * Flow control manager for timestamped multiplayer message streams, trading latency for timeflow consistency.
 */
#pragma once
#include "util/types.hpp"

namespace sky {

namespace detail {

/**
 * Component of FlowControl defined independently of the Message type.
 */
class FlowControlBase {
 public:
  FlowControlBase()

};


}

/**
 * Settings governing the nature of the flexible flow control algorithm.
 */
struct FlowControlSettings {
  FlowControlSettings(); // initialize with sensible defaults

};

/**
 * FlowControl class, templated on the Message type. Push messages in chronological order and pull them.
 */
template<typename Message>
class FlowControl {
 private:
  const FlowControlSettings settings;
  std::vector<std::pair<Time, Message>> messages;
  detail::FlowControlBase base;

 public:
  FlowControl(const FlowControlSettings &setttings) : settings(settings) {}

  // Push a message received with the specified timestamp.
  // Messages must be pushed in chronological order!
  void push(const Time timestamp, Message &&message) {
    messages.push_back
  }

  // Pull aged messages. It's important to age the messages properly for their digital aromas to mature.
  // Returns nullptr when no messages are available. Poll this function.
  Message *pull() {
    return nullptr;
  }



};

}
