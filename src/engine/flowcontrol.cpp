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
#include "flowcontrol.hpp"
#include "util/methods.hpp"

namespace sky {

/**
 * FlowState.
 */

FlowState::FlowState() :
  offsets(500) {}

void FlowState::registerArrival(const Time offset) {
  offsets.push(offset);
  if (window) {
    approach<Time>(*window, offsets.max(), 0.005);
  } else {
    window = offsets.max();
  }
}

bool FlowState::release(const Time offset) {
  return !window or (window and offset > *window);
}

}

