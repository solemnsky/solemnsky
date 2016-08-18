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
 * FlowControlSettings.
 */
FlowControlSettings::FlowControlSettings() { }

namespace detail {

bool pullMessage(const FlowControlSettings &settings,
                 const Time localtime,
                 const Time timestamp) {
  if (settings.windowEntry) {
    if (settings.windowSize) {
      return localtime - timestamp > *settings.windowEntry;
    } else {
      return inRange<Time>(
          (localtime - timestamp) - *settings.windowEntry,
          0, *settings.windowSize);
    }
  }
  return true;
}

}

}